#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct user {
    char phone[50];
    char ac[50];
    char password[50];
    float balance;
};

// Generate 10-digit account number starting with '4'
void generateAccountNumber(char ac[]) {
    srand((unsigned int)time(NULL) ^ rand());
    strcpy(ac, "4");
    for (int i = 0; i < 9; i++) {
        char digit = '0' + (rand() % 10);
        strncat(ac, &digit, 1);
    }
}

// Log transaction with real timestamp
void logTransaction(const char* phone, const char* type, float amount, float balance) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_statements.txt", phone);

    FILE *fp = fopen(filename, "a");
    if (fp) {
        time_t now = time(NULL);
        struct tm *tm_info = localtime(&now);
        char buffer[100];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(fp, "[%s] %-15s | Amount: K.%.2f | Balance: K.%.2f\n", buffer, type, amount, balance);
        fclose(fp);
    }
}

// View transaction statements
void viewStatements(const char* phone) {
    char filename[100];
    snprintf(filename, sizeof(filename), "%s_statements.txt", phone);

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("\nNo transaction history found.\n");
        return;
    }

    printf("\n--- TRANSACTION STATEMENTS ---\n");
    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        printf("%s", line);
        count++;
    }
    if (count == 0) {
        printf("No transactions recorded.\n");
    }
    fclose(fp);
}

int main() {
    struct user usr, usr1;
    FILE *fp;
    char filename[100], phone[50], pword[50];
    int opt, choice;
    char cont = 'y';
    float amount;

    printf("\n🏦 SWISS ONLINE BANKING 🏦\n");
    printf("\n1. Register an account");
    printf("\n2. Login to an account");
    printf("\n\nYour choice: ");
    scanf("%d", &opt);

    if (opt == 1) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("Enter your phone number (e.g., 07XXXXXXXX): ");
        scanf("%s", usr.phone);
        printf("Enter your new password: ");
        scanf("%s", usr.password);
        usr.balance = 0.0f;

        generateAccountNumber(usr.ac);
        printf("\n✅ Your account number: %s\n", usr.ac);

        snprintf(filename, sizeof(filename), "%s.dat", usr.phone);
        fp = fopen(filename, "wb");
        if (!fp) {
            printf("\n❌ Error: Could not create account. Try again.\n");
            return 1;
        }
        fwrite(&usr, sizeof(struct user), 1, fp);
        fclose(fp);

        // Create empty statement file
        snprintf(filename, sizeof(filename), "%s_statements.txt", usr.phone);
        fp = fopen(filename, "w");
        if (fp) fclose(fp);

        printf("\n🎉 Account successfully registered!\n");
    }
    else if (opt == 2) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        printf("Phone number: ");
        scanf("%s", phone);
        printf("Password: ");
        scanf("%s", pword);

        snprintf(filename, sizeof(filename), "%s.dat", phone);
        fp = fopen(filename, "rb");
        if (!fp) {
            printf("\n❌ Account not found. Please register first.\n");
            return 1;
        }

        fread(&usr, sizeof(struct user), 1, fp);
        fclose(fp);

        if (strcmp(pword, usr.password) != 0) {
            printf("\n❌ Invalid password!\n");
            return 1;
        }

        printf("\n✅ Welcome, %s!\n", usr.phone);

        while (cont == 'y' || cont == 'Y') {
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            printf("\n--- MAIN MENU ---\n");
            printf("1. Balance Inquiry\n");
            printf("2. Deposit Cash\n");
            printf("3. Withdraw Cash\n");
            printf("4. Online Transfer\n");
            printf("5. Change Password\n");
            printf("6. Bitcoin / Cryptocurrency\n");
            printf("7. View Transaction Statements\n");
            printf("\nChoose an option: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1:
                    printf("\n💰 Your current balance: K.%.2f\n", usr.balance);
                    break;

                case 2:
                    printf("\nEnter deposit amount: K.");
                    scanf("%f", &amount);
                    if (amount <= 0) {
                        printf("\n❌ Invalid amount.\n");
                    } else {
                        usr.balance += amount;
                        snprintf(filename, sizeof(filename), "%s.dat", usr.phone);
                        fp = fopen(filename, "rb+");
                        if (fp) {
                            fseek(fp, 0, SEEK_SET);
                            fwrite(&usr, sizeof(struct user), 1, fp);
                            fclose(fp);
                            logTransaction(usr.phone, "DEPOSIT", amount, usr.balance);
                            printf("\n✅ Successfully deposited K.%.2f\n", amount);
                        }
                    }
                    break;

                case 3:
                    printf("\nEnter withdrawal amount: K.");
                    scanf("%f", &amount);
                    if (amount <= 0 || amount > usr.balance) {
                        printf("\n❌ Insufficient funds or invalid amount.\n");
                    } else {
                        usr.balance -= amount;
                        snprintf(filename, sizeof(filename), "%s.dat", usr.phone);
                        fp = fopen(filename, "rb+");
                        if (fp) {
                            fseek(fp, 0, SEEK_SET);
                            fwrite(&usr, sizeof(struct user), 1, fp);
                            fclose(fp);
                            logTransaction(usr.phone, "WITHDRAWAL", amount, usr.balance);
                            printf("\n✅ Withdrawn K.%.2f\n", amount);
                        }
                    }
                    break;

                case 4: {
                    char recipient[50];
                    printf("\nRecipient's phone number: ");
                    scanf("%s", recipient);
                    printf("Amount to transfer: K.");
                    scanf("%f", &amount);

                    if (amount <= 0 || amount > usr.balance) {
                        printf("\n❌ Invalid amount or insufficient balance.\n");
                        break;
                    }

                    char recvFile[100];
                    snprintf(recvFile, sizeof(recvFile), "%s.dat", recipient);
                    fp = fopen(recvFile, "rb");
                    if (!fp) {
                        printf("\n❌ Recipient account not found.\n");
                        break;
                    }

                    fread(&usr1, sizeof(struct user), 1, fp);
                    fclose(fp);

                    // Update recipient
                    usr1.balance += amount;
                    fp = fopen(recvFile, "rb+");
                    if (fp) {
                        fseek(fp, 0, SEEK_SET);
                        fwrite(&usr1, sizeof(struct user), 1, fp);
                        fclose(fp);
                        logTransaction(recipient, "RECEIVED", amount, usr1.balance);
                    }

                    // Update sender
                    usr.balance -= amount;
                    snprintf(filename, sizeof(filename), "%s.dat", usr.phone);
                    fp = fopen(filename, "rb+");
                    if (fp) {
                        fseek(fp, 0, SEEK_SET);
                        fwrite(&usr, sizeof(struct user), 1, fp);
                        fclose(fp);
                        logTransaction(usr.phone, "TRANSFER OUT", amount, usr.balance);
                    }

                    printf("\n✅ Transferred K.%.2f to %s\n", amount, recipient);
                    break;
                }

                case 5:
                    printf("\nEnter new password: ");
                    scanf("%s", pword);
                    strcpy(usr.password, pword);
                    snprintf(filename, sizeof(filename), "%s.dat", usr.phone);
                    fp = fopen(filename, "rb+");
                    if (fp) {
                        fseek(fp, 0, SEEK_SET);
                        fwrite(&usr, sizeof(struct user), 1, fp);
                        fclose(fp);
                        printf("\n✅ Password changed successfully.\n");
                    }
                    break;

                case 6: {
                    const float BTC_RATE = 50000.0f;
                    printf("\n--- ₿ Bitcoin Conversion ---\n");
                    printf("Your balance: K.%.2f\n", usr.balance);
                    printf("Rate: 1 BTC = K.%.2f\n", BTC_RATE);
                    printf("Max BTC you can buy: %.6f\n", usr.balance / BTC_RATE);

                    printf("\nConvert entire balance to BTC? (y/n): ");
                    char confirm;
                    scanf(" %c", &confirm);
                    if (confirm == 'y' || confirm == 'Y') {
                        if (usr.balance > 0) {
                            float btc = usr.balance / BTC_RATE;
                            printf("\n✅ Converted K.%.2f to %.6f BTC (simulated)\n", usr.balance, btc);
                            logTransaction(usr.phone, "BTC CONVERT", usr.balance, usr.balance);
                        } else {
                            printf("\n❌ No balance to convert.\n");
                        }
                    }
                    break;
                }

                case 7:
                    viewStatements(usr.phone);
                    break;

                default:
                    printf("\n⚠️ Invalid option.\n");
                    break;
            }

            printf("\nContinue? (y/n): ");
            scanf(" %c", &cont);
        }
    }
    else {
        printf("\n⚠️ Invalid choice. Exiting...\n");
    }

    printf("\n👋 Thank you for banking with us!\n");
    return 0;
}
