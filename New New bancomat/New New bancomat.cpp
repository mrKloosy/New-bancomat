#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
#include <conio.h>
#include <map>
#include <iomanip>
#include <algorithm>
#pragma warning(disable : 4996)
using namespace std;

enum Keys {
    Up = 72,
    Down = 80,
    Enter = 13,
    Esc = 27
};

struct Transaction {
    time_t timestamp;
    double amount;
    bool isDeposit;
};

class Card 
{
    string cardNumber;
    string pin;

public:
    Card(string number, string pin) : cardNumber(number), pin(pin) {}

    string getCardNumber() const 
    {
        return cardNumber;
    }

    string getPin() const 
    {
        return pin;
    }
};

class Account 
{
    vector<Transaction> transactions;
    Card card;
    double balance;

public:
    Account(Card card) : card(card), balance(0) {}

    void deposit(double amount) 
    {
        Transaction transaction;
        transaction.timestamp = time(nullptr);
        transaction.amount = amount;
        transaction.isDeposit = true;
        transactions.push_back(transaction);
        balance += amount;
    }

    void withdraw(double amount) 
    {
        Transaction transaction;
        transaction.timestamp = time(nullptr);
        transaction.amount = amount;
        transaction.isDeposit = false;
        transactions.push_back(transaction);
        balance -= amount;
    }

    double getBalance() const 
    {
        return balance;
    }

    vector<Transaction> getTransactions() const 
    {
        return transactions;
    }

    const Card& getCard() const 
    {
        return card;
    }
};

void printMenu(vector<string> str, int punkt) 
{
    int X = 15, Y = 5;
    for (size_t i = 0; i < str.size(); i++) 
    {
        if (punkt == i) 
        {
            cout << "> ";
        }
        else 
        {
            cout << "  ";
        }
        cout << str[i] << endl;
    }
}

int Menu(vector<string> str) 
{
    int key = 0, punktOfMenu = 0;
    do 
    {
        system("cls");
        printMenu(str, punktOfMenu);
        key = _getch();
        switch (key) 
        {
        case Down:
            if (punktOfMenu < static_cast<int>(str.size()) - 1) 
            {
                punktOfMenu++;
            }
            break;
        case Up:
            if (punktOfMenu > 0) 
            {
                punktOfMenu--;
            }
            break;
        case Enter:
            return punktOfMenu;
            break;
        }
    } while (key != Esc);
    return -1;
}


class BankingApp 
{
    vector<Account> accounts;
public:
    BankingApp() 
    {
        loadAccounts();
    }

    void loadAccounts() 
    {
        ifstream file("accounts.txt");
        if (file.is_open()) 
        {
            while (!file.eof()) 
            {
                string cardNumber, pin;
                file >> cardNumber >> pin;
                Account account(Card(cardNumber, pin));
                loadTransactions(account);
                accounts.push_back(account);
            }
            file.close();
        }
    }

    void saveTransactions(const Account& account) 
    {
        string fileName = account.getCard().getCardNumber() + "_transactions.txt";
        ofstream file(fileName);
        if (file.is_open()) 
        {
            for (const auto& transaction : account.getTransactions()) 
            {
                file << transaction.timestamp << " " << transaction.amount << " " << transaction.isDeposit << endl;
            }
            file.close();
        }
    }

    void loadTransactions(Account& account) 
    {
        string fileName = account.getCard().getCardNumber() + "_transactions.txt";
        ifstream file(fileName);
        if (file.is_open()) 
        {
            Transaction transaction;
            while (file >> transaction.timestamp >> transaction.amount >> transaction.isDeposit) 
            {
                account.getTransactions().push_back(transaction);
                if (transaction.isDeposit)
                {
                    account.deposit(transaction.amount);
                }
                else
                {
                    account.withdraw(transaction.amount);
                }
            }
            file.close();
        }
    }

    void saveAccounts() 
    {
        ofstream file("accounts.txt");
        if (file.is_open()) 
        {
            for (const auto& account : accounts) 
            {
                file << account.getCard().getCardNumber() << " " << account.getCard().getPin() << endl;
            }
            file.close();
        }
    }

    Account* findAccount(const string& cardNumber, const string& pin) 
    {
        for (auto& account : accounts) 
        {
            if (account.getCard().getCardNumber() == cardNumber and account.getCard().getPin() == pin) 
            {
                return &account;
            }
        }
        return nullptr;
    }

    void createAccount(const string& cardNumber, const string& pin) 
    {
        Account account(Card(cardNumber, pin));
        accounts.push_back(account);
        saveAccounts();
    }

    void deposit(Account& account, double amount) 
    {
        account.deposit(amount);
        saveTransactions(account);
    }

    void withdraw(Account& account, double amount) 
    {
        account.withdraw(amount);
        saveTransactions(account);
    }

    void viewTransactions(const Account& account) 
    {
        vector<Transaction> transactions = account.getTransactions();
        system("cls");
        cout << "История транзакций:" << endl;
        for (const auto& transaction : transactions)
        {
            struct tm* timeinfo;
            timeinfo = localtime(&transaction.timestamp);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            cout << (transaction.isDeposit ? "Внесение: " : "Снятие: ") << transaction.amount << " (время: " << buffer << ")" << endl;
        }
        system("pause");
    }

    void viewBalance(const Account& account)
    {
        system("cls");
        cout << "Текущий баланс: " << account.getBalance() << endl;
        system("pause");
    }

    void viewTopExpenses(const Account& account, int n) 
    {
        map<string, double> expenses;
        for (const auto& transaction : account.getTransactions()) 
        {
            string category = transaction.isDeposit ? "Депозит" : "Снятие";
            expenses[category] += transaction.amount;
        }
        vector<pair<string, double>> sortedExpenses(expenses.begin(), expenses.end());
        sort(sortedExpenses.begin(), sortedExpenses.end(), [](const auto& a, const auto& b) 
            {
            return a.second > b.second;
            });

        system("cls");
        cout << "Топ-" << n << " расходов:" << endl;
        for (int i = 0; i < min(n, static_cast<int>(sortedExpenses.size())); ++i) 
        {
            cout << sortedExpenses[i].first << ": " << sortedExpenses[i].second << endl;
        }
        system("pause");
    }

    void run()
    {
        int choice;
        string cardNumber, pin;

        while (true) 
        {
            cout << "Добро пожаловать в банковское приложение" << endl;
            cout << "1. Войти" << endl;
            cout << "2. Создать аккаунт" << endl;
            cout << "3. Выйти" << endl;
            cout << "Выберите: ";
            cin >> choice;

            if (choice == 1) 
            {
                cout << "Введите номер карты: ";
                cin >> cardNumber;
                cout << "Введите PIN: ";
                cin >> pin;
                Account* account = findAccount(cardNumber, pin);
                if (account) {
                    cout << "Вход выполнен успешно!" << endl;
                    vector<string> menuOptions =
                    {
                        "Внести",
                        "Снять",
                        "Просмотр транзакций",
                        "Просмотр баланса",
                        "Топ расходов",
                        "Выход"
                    };

                    int selection;
                    do 
                    {
                        selection = Menu(menuOptions);
                        switch (selection) 
                        {
                        case 0: 
                        {
                            double amount;
                            cout << "Введите сумму для внесения: ";
                            cin >> amount;
                            deposit(*account, amount);
                            break;
                        }
                        case 1: 
                        {
                            double amount;
                            cout << "Введите сумму для снятия: ";
                            cin >> amount;
                            withdraw(*account, amount);
                            break;
                        }
                        case 2:
                            viewTransactions(*account);
                            break;
                        case 3:
                            viewBalance(*account);
                            break;
                        case 4: 
                        {
                            int n;
                            cout << "Введите количество транзакций: ";
                            cin >> n;
                            viewTopExpenses(*account, n);
                            break;
                        }
                        case 5:
                            exit(0);
                            break;
                        }
                    } while (selection != -1);
                }
                else 
                {
                    cout << "Неверный номер карты или PIN. Пожалуйста, повторите попытку." << endl;
                }
            }
            else if (choice == 2) 
            {
                cout << "Введите номер карты: ";
                cin >> cardNumber;
                cout << "Введите PIN: ";
                cin >> pin;
                createAccount(cardNumber, pin);
                system("cls");
                cout << "Аккаунт успешно создан!" << endl;
            }
            else if (choice == 3) 
            {
                break;
            }
            else
            {
                cout << "Неверный выбор. Пожалуйста, повторите попытку." << endl;
            }
        }
    }
};

int main() 
{
    setlocale(LC_ALL, "rus");
    BankingApp app;
    app.run();
}