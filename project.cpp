#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
using namespace std;

// ======= File path for student data =======
const string FILE_PATH = R"(Students.txt)";

// ======= Class representing an item in the menu =======
class Item {
public:
    string name;
    double price;

    Item(string n = "", double p = 0) : name(n), price(p) {}
};

// ======= Class representing a category in the menu =======
class Category {
public:
    string categoryName;
    vector<Item> items;

    Category(string name = "") : categoryName(name) {}

    void addItem(Item item) {
        items.push_back(item);
    }

    void displayCategory() {
        cout << "\n===== " << categoryName << " =====\n";
        for (size_t i = 0; i < items.size(); i++)
            cout << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price << "\n";
    }
};

// ======= Class representing the menu =======
class Menu {
public:
    vector<Category> categories;

    Menu() {
        // Create Savoury category
        Category savoury("Savoury");
        savoury.addItem(Item("Sandwich", 350));
        savoury.addItem(Item("Burger", 400));
        savoury.addItem(Item("Pizza Slice", 450));
        
        // Create Sweets category
        Category sweets("Sweets");
        sweets.addItem(Item("Cookie", 250));
        sweets.addItem(Item("Brownie", 300));
        sweets.addItem(Item("Donut", 200));
        
        // Create Beverages category
        Category beverages("Beverages");
        beverages.addItem(Item("Coffee", 400));
        beverages.addItem(Item("Pepsi", 60));
        beverages.addItem(Item("Tea", 150));
        beverages.addItem(Item("Juice", 200));

        categories.push_back(savoury);
        categories.push_back(sweets);
        categories.push_back(beverages);
    }

    void displayCategories() {
        cout << "\n===== MENU CATEGORIES =====\n";
        for (size_t i = 0; i < categories.size(); i++)
            cout << i + 1 << ". " << categories[i].categoryName << "\n";
    }

    void displayCategoryMenu(int categoryIndex) {
        if (categoryIndex >= 0 && categoryIndex < (int)categories.size()) {
            categories[categoryIndex].displayCategory();
        }
    }
};

// ======= Class representing shopping cart =======
class ShoppingCart {
public:
    vector<Item> items;

    void addItem(Item item) {
        items.push_back(item);
        cout << "✅ Added " << item.name << " to cart.\n";
    }

    void removeItem(int index) {
        if (index < 0 || index >= (int)items.size()) {
            cout << "❌ Invalid index.\n";
            return;
        }
        cout << "❌ Removed " << items[index].name << " from cart.\n";
        items.erase(items.begin() + index);
    }

    double getTotal() {
        double total = 0;
        for (auto &item : items)
            total += item.price;
        return total;
    }

    void displayCart() {
        cout << "\n===== YOUR SHOPPING CART =====\n";
        if (items.empty()) {
            cout << "Cart is empty!\n";
            return;
        }
        for (size_t i = 0; i < items.size(); i++)
            cout << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price << "\n";
        cout << "------------------------------\n";
        cout << "Total: Rs. " << getTotal() << "\n";
    }

    void clearCart() {
        items.clear();
    }

    bool isEmpty() {
        return items.empty();
    }
};

// ======= Base class for Payment (Abstract) =======
class Payment {
protected:
    double amount;
    string transactionID;

public:
    Payment(double amt) : amount(amt) {
        srand(time(0));
        transactionID = "TXN" + to_string(rand() % 100000);
    }

    virtual ~Payment() {}

    virtual bool processPayment() = 0; // Pure virtual function
    virtual void displayPaymentDetails() = 0;

    string getTransactionID() {
        return transactionID;
    }
};

// ======= Cash Payment (Child of Payment) =======
class CashPayment : public Payment {
public:
    CashPayment(double amt) : Payment(amt) {}

    bool processPayment() override {
        cout << "\n💵 Processing Cash Payment...\n";
        cout << "Amount: Rs. " << amount << "\n";
        cout << "✅ Cash payment successful!\n";
        return true;
    }

    void displayPaymentDetails() override {
        cout << "\n========== PAYMENT RECEIPT ==========\n";
        cout << "Payment Method: Cash\n";
        cout << "Amount Paid: Rs. " << amount << "\n";
        cout << "Transaction ID: " << transactionID << "\n";
        cout << "=====================================\n";
    }
};

// ======= Card Payment (Child of Payment) =======
class CardPayment : public Payment {
private:
    string cardNumber;

public:
    CardPayment(double amt, string card) : Payment(amt), cardNumber(card) {}

    bool processPayment() override {
        cout << "\n💳 Processing Card Payment...\n";
        cout << "Card Number: XXXX-XXXX-XXXX-" << cardNumber.substr(cardNumber.length() - 4) << "\n";
        cout << "Amount: Rs. " << amount << "\n";
        cout << "✅ Card payment successful!\n";
        return true;
    }

    void displayPaymentDetails() override {
        cout << "\n========== PAYMENT RECEIPT ==========\n";
        cout << "Payment Method: Card\n";
        cout << "Card: XXXX-XXXX-XXXX-" << cardNumber.substr(cardNumber.length() - 4) << "\n";
        cout << "Amount Paid: Rs. " << amount << "\n";
        cout << "Transaction ID: " << transactionID << "\n";
        cout << "=====================================\n";
    }
};

// ======= Online Payment (Child of Payment) =======
class OnlinePayment : public Payment {
private:
    string email;

public:
    OnlinePayment(double amt, string userEmail) : Payment(amt), email(userEmail) {}

    bool processPayment() override {
        cout << "\n🌐 Processing Online Payment...\n";
        cout << "Email: " << email << "\n";
        cout << "Amount: Rs. " << amount << "\n";
        cout << "✅ Online payment successful!\n";
        return true;
    }

    void displayPaymentDetails() override {
        cout << "\n========== PAYMENT RECEIPT ==========\n";
        cout << "Payment Method: Online\n";
        cout << "Email: " << email << "\n";
        cout << "Amount Paid: Rs. " << amount << "\n";
        cout << "Transaction ID: " << transactionID << "\n";
        cout << "=====================================\n";
    }
};

// ======= Balance Class (manages user's money) =======
class Balance {
private:
    double totalBalance;
    vector<string> transactionHistory;

public:
    Balance(double initial = 1000) : totalBalance(initial) {
        transactionHistory.push_back("Initial balance: Rs. " + to_string(initial));
    }

    double getBalance() {
        return totalBalance;
    }

    void displayBalance() {
        cout << "\n💰 Current Balance: Rs. " << totalBalance << "\n";
    }

    bool deductAmount(double amount, string method) {
        if (amount > totalBalance) {
            cout << "❌ Insufficient balance!\n";
            return false;
        }
        totalBalance -= amount;
        transactionHistory.push_back("Deducted Rs. " + to_string(amount) + " via " + method);
        return true;
    }

    void addAmount(double amount, string method) {
        totalBalance += amount;
        transactionHistory.push_back("Added Rs. " + to_string(amount) + " via " + method);
    }

    void displayTransactionHistory() {
        cout << "\n========== TRANSACTION HISTORY ==========\n";
        for (const auto& transaction : transactionHistory) {
            cout << "• " << transaction << "\n";
        }
        cout << "=========================================\n";
    }

    bool withdrawCash(double amount) {
        if (deductAmount(amount, "Cash Withdrawal")) {
            cout << "✅ Withdrew Rs. " << amount << " in cash.\n";
            return true;
        }
        return false;
    }

    bool withdrawViaCard(double amount) {
        if (deductAmount(amount, "Card Withdrawal")) {
            cout << "✅ Withdrew Rs. " << amount << " via card.\n";
            return true;
        }
        return false;
    }

    bool withdrawOnline(double amount) {
        if (deductAmount(amount, "Online Transfer")) {
            cout << "✅ Transferred Rs. " << amount << " online.\n";
            return true;
        }
        return false;
    }

    void depositCash(double amount) {
        addAmount(amount, "Cash Deposit");
        cout << "✅ Deposited Rs. " << amount << " in cash.\n";
    }

    void depositViaCard(double amount) {
        addAmount(amount, "Card Deposit");
        cout << "✅ Deposited Rs. " << amount << " via card.\n";
    }

    void depositOnline(double amount) {
        addAmount(amount, "Online Deposit");
        cout << "✅ Deposited Rs. " << amount << " online.\n";
    }
};

// ======= Class representing student (user) =======
class Student {
public:
    string id;
    string password;
    Balance balance;
    ShoppingCart cart;

    Student() : balance(1000) {}

    void inputDetails() {
        cout << "Enter Student ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;
        double initialBalance;
        cout << "Enter Initial Balance: ";
        cin >> initialBalance;
        balance = Balance(initialBalance);
    }

    void saveToFile() {
        ofstream outFile(FILE_PATH, ios::app);
        if (!outFile) {
            cout << "❌ Error opening file!\n";
            return;
        }
        outFile << id << " " << password << " " << balance.getBalance() << endl;
        outFile.close();
        cout << "✅ Account saved successfully!\n";
    }

    bool login() {
        string loginID, loginPass;
        cout << "Enter your ID: ";
        cin >> loginID;
        cout << "Enter your password: ";
        cin >> loginPass;

        ifstream inFile(FILE_PATH);
        if (!inFile) {
            cout << "❌ No user data found.\n";
            return false;
        }

        string storedID, storedPass;
        double storedBalance;
        while (inFile >> storedID >> storedPass >> storedBalance) {
            if (loginID == storedID && loginPass == storedPass) {
                cout << "✅ Login successful! Welcome, " << loginID << "!\n";
                id = storedID;
                password = storedPass;
                balance = Balance(storedBalance);
                inFile.close();
                return true;
            }
        }

        inFile.close();
        cout << "❌ Invalid ID or password.\n";
        return false;
    }

    void updateBalanceInFile() {
        ifstream inFile(FILE_PATH);
        ofstream outFile("temp.txt");
        string storedID, storedPass;
        double storedBalance;
        while (inFile >> storedID >> storedPass >> storedBalance) {
            if (storedID == id) {
                storedBalance = balance.getBalance();
            }
            outFile << storedID << " " << storedPass << " " << storedBalance << endl;
        }
        inFile.close();
        outFile.close();
        remove(FILE_PATH.c_str());
        rename("temp.txt", FILE_PATH.c_str());
    }

    void checkout() {
        if (cart.isEmpty()) {
            cout << "❌ Cart is empty! Add items first.\n";
            return;
        }

        double total = cart.getTotal();
        cout << "\n========== CHECKOUT ==========\n";
        cout << "Total Amount: Rs. " << total << "\n";
        balance.displayBalance();

        if (total > balance.getBalance()) {
            cout << "❌ Not enough balance! Please top-up your account.\n";
            return;
        }

        cout << "\nSelect Payment Method:\n";
        cout << "1. Cash\n";
        cout << "2. Card\n";
        cout << "3. Online\n";
        cout << "Enter choice: ";
        int paymentChoice;
        cin >> paymentChoice;

        Payment* payment = nullptr;

        switch (paymentChoice) {
            case 1:
                payment = new CashPayment(total);
                break;
            case 2: {
                string cardNum;
                cout << "Enter card number (16 digits): ";
                cin >> cardNum;
                payment = new CardPayment(total, cardNum);
                break;
            }
            case 3: {
                string email;
                cout << "Enter email: ";
                cin >> email;
                payment = new OnlinePayment(total, email);
                break;
            }
            default:
                cout << "❌ Invalid payment method!\n";
                return;
        }

        if (payment && payment->processPayment()) {
            string method = (paymentChoice == 1) ? "Cash Payment" : 
                           (paymentChoice == 2) ? "Card Payment" : "Online Payment";
            balance.deductAmount(total, method);
            payment->displayPaymentDetails();
            cout << "✅ Order placed successfully!\n";
            balance.displayBalance();
            cart.clearCart();
            updateBalanceInFile();
        }

        delete payment;
    }
};

// ======= MAIN FUNCTION =======
int main() {
    Menu menu;
    Student student;
    int choice;

    cout << "\n";
    cout << "╔════════════════════════════════════════╗\n";
    cout << "║     🍰 GrabNob Bakery System 🍰       ║\n";
    cout << "║   Your Campus Food Ordering App       ║\n";
    cout << "╚════════════════════════════════════════╝\n\n";
    
    cout << "1. Register New Account\n";
    cout << "2. Login\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) {
        cout << "\n===== REGISTRATION =====\n";
        student.inputDetails();
        student.saveToFile();
        cout << "\nPlease login to continue...\n";
        return 0;
    } else if (choice == 2) {
        cout << "\n===== LOGIN =====\n";
        if (!student.login()) {
            return 0;
        }
    } else {
        cout << "❌ Invalid choice.\n";
        return 0;
    }

    // ======= After login: Main Menu =======
    do {
        cout << "\n╔════════════════════════════════════════╗\n";
        cout << "║         🍰 GrabNob Bakery 🍰          ║\n";
        cout << "╚════════════════════════════════════════╝\n";
        student.balance.displayBalance();
        cout << "\n1. Browse Menu (by Category)\n";
        cout << "2. View Shopping Cart\n";
        cout << "3. Remove Item from Cart\n";
        cout << "4. Checkout\n";
        cout << "5. Manage Balance\n";
        cout << "6. View Transaction History\n";
        cout << "0. Logout\n";
        cout << "\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                // Browse menu by category
                menu.displayCategories();
                cout << "\nSelect category (0 to go back): ";
                int cat;
                cin >> cat;
                if (cat > 0 && cat <= (int)menu.categories.size()) {
                    menu.displayCategoryMenu(cat - 1);
                    cout << "\nEnter item number to add to cart (0 to go back): ";
                    int itemNum;
                    cin >> itemNum;
                    if (itemNum > 0 && itemNum <= (int)menu.categories[cat - 1].items.size()) {
                        student.cart.addItem(menu.categories[cat - 1].items[itemNum - 1]);
                    }
                }
                break;
            }
            case 2:
                student.cart.displayCart();
                break;
            case 3: {
                student.cart.displayCart();
                if (!student.cart.isEmpty()) {
                    int removeIndex;
                    cout << "\nEnter item number to remove (0 to cancel): ";
                    cin >> removeIndex;
                    if (removeIndex > 0) {
                        student.cart.removeItem(removeIndex - 1);
                    }
                }
                break;
            }
            case 4:
                student.checkout();
                break;
            case 5: {
                cout << "\n===== MANAGE BALANCE =====\n";
                cout << "1. Deposit Money\n";
                cout << "2. Withdraw Money\n";
                cout << "3. View Balance\n";
                cout << "Enter choice: ";
                int balChoice;
                cin >> balChoice;
                
                if (balChoice == 1) {
                    cout << "\nSelect deposit method:\n";
                    cout << "1. Cash\n2. Card\n3. Online\n";
                    cout << "Enter choice: ";
                    int method;
                    cin >> method;
                    cout << "Enter amount: Rs. ";
                    double amt;
                    cin >> amt;
                    if (method == 1) student.balance.depositCash(amt);
                    else if (method == 2) student.balance.depositViaCard(amt);
                    else if (method == 3) student.balance.depositOnline(amt);
                    student.updateBalanceInFile();
                } else if (balChoice == 2) {
                    cout << "\nSelect withdrawal method:\n";
                    cout << "1. Cash\n2. Card\n3. Online\n";
                    cout << "Enter choice: ";
                    int method;
                    cin >> method;
                    cout << "Enter amount: Rs. ";
                    double amt;
                    cin >> amt;
                    if (method == 1) student.balance.withdrawCash(amt);
                    else if (method == 2) student.balance.withdrawViaCard(amt);
                    else if (method == 3) student.balance.withdrawOnline(amt);
                    student.updateBalanceInFile();
                } else if (balChoice == 3) {
                    student.balance.displayBalance();
                }
                break;
            }
            case 6:
                student.balance.displayTransactionHistory();
                break;
            case 0:
                cout << "\n👋 Logging out... Thank you for using GrabNob!\n";
                break;
            default:
                cout << "❌ Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}