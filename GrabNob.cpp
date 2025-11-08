#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

// ======= File path for student data =======
const string FILE_PATH = R"(C:\Users\SBT\OOP Spring\OOP Project\file for login data\Students.txt)";

// ======= Class representing an item in the menu =======
class Item {
public:
    string name;
    double price;

    Item(string n, double p) : name(n), price(p) {}
};

// ======= Class representing the menu =======
class Menu {
public:
    vector<Item> savoury;
    vector<Item> sweets;
    vector<Item> beverages;

    Menu() {
        // ======= Add your menu items here =======
        savoury.push_back(Item("Sandwich", 350));
        savoury.push_back(Item("Burger", 400));

        sweets.push_back(Item("Cookie", 250));

        beverages.push_back(Item("Coffee", 400));
        beverages.push_back(Item("Pepsi", 60));
        // ========================================
    }

    void displayMenu() {
        cout << "===== MENU =====\n";
        cout << "\n-- Savoury --\n";
        for (size_t i = 0; i < savoury.size(); i++)
            cout << i + 1 << ". " << savoury[i].name << " - " << savoury[i].price << " PKR\n";

        cout << "\n-- Sweets --\n";
        for (size_t i = 0; i < sweets.size(); i++)
            cout << i + 1 << ". " << sweets[i].name << " - " << sweets[i].price << " PKR\n";

        cout << "\n-- Beverages --\n";
        for (size_t i = 0; i < beverages.size(); i++)
            cout << i + 1 << ". " << beverages[i].name << " - " << beverages[i].price << " PKR\n";
    }
};

// ======= Class representing shopping cart =======
class Cart {
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
        cout << "\n===== YOUR CART =====\n";
        for (size_t i = 0; i < items.size(); i++)
            cout << i + 1 << ". " << items[i].name << " - " << items[i].price << " PKR\n";
        cout << "Total: " << getTotal() << " PKR\n";
    }
};

// ======= Class representing student (has-a cart) =======
class Student {
public:
    string id;
    string password;
    double balance;
    Cart cart;

    Student() : balance(1000) {} // default initial balance

    // input registration details
    void inputDetails() {
        cout << "Enter Student ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;
        cout << "Enter Initial Balance: ";
        cin >> balance;
    }

    // save registration to file
    void saveToFile() {
        ofstream outFile(FILE_PATH, ios::app);
        if (!outFile) {
            cout << "Error opening file!\n";
            return;
        }
        outFile << id << " " << password << " " << balance << endl;
        outFile.close();
        cout << "✅ Account saved successfully!\n";
    }

    // login function
    bool login() {
        string loginID, loginPass;
        cout << "Enter your ID: ";
        cin >> loginID;
        cout << "Enter your password: ";
        cin >> loginPass;

        ifstream inFile(FILE_PATH);
        if (!inFile) {
            cout << "No user data found.\n";
            return false;
        }

        string storedID, storedPass;
        double storedBalance;
        while (inFile >> storedID >> storedPass >> storedBalance) {
            if (loginID == storedID && loginPass == storedPass) {
                cout << "✅ Login successful! Welcome, " << loginID << "!\n";
                id = storedID;
                password = storedPass;
                balance = storedBalance;
                inFile.close();
                return true;
            }
        }

        inFile.close();
        cout << "❌ Invalid ID or password.\n";
        return false;
    }

    // save updated balance back to file
    void updateBalanceInFile() {
        ifstream inFile(FILE_PATH);
        ofstream outFile("temp.txt");
        string storedID, storedPass;
        double storedBalance;
        while (inFile >> storedID >> storedPass >> storedBalance) {
            if (storedID == id) {
                storedBalance = balance;
            }
            outFile << storedID << " " << storedPass << " " << storedBalance << endl;
        }
        inFile.close();
        outFile.close();
        remove(FILE_PATH.c_str());
        rename("temp.txt", FILE_PATH.c_str());
    }

    // checkout cart
    void checkout() {
        double total = cart.getTotal();
        if (total > balance) {
            cout << "❌ Not enough balance! Please top-up.\n";
        } else {
            balance -= total;
            cout << "✅ Order placed successfully! Remaining balance: " << balance << " PKR\n";
            cart.items.clear(); // empty cart after checkout
            updateBalanceInFile();
        }
    }
};

// ======= MAIN =======
int main() {
    Menu menu;
    Student student;
    int choice;

    cout << "====== GrabNob Bakery Login System ======\n";
    cout << "1. Register new student\n";
    cout << "2. Login\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 1) {
        student.inputDetails();
        student.saveToFile();
    } else if (choice == 2) {
        if (!student.login()) {
            return 0; // exit if login fails
        }
    } else {
        cout << "Invalid choice.\n";
        return 0;
    }

    // ======= After login: Menu and Cart system =======
    do {
        cout << "\n===== GrabNob Bakery =====\n";
        cout << "Balance: " << student.balance << " PKR\n";
        cout << "1. Show Menu\n";
        cout << "2. Add Item to Cart\n";
        cout << "3. Remove Item from Cart\n";
        cout << "4. Show Cart\n";
        cout << "5. Checkout\n";
        cout << "0. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                menu.displayMenu();
                break;
            case 2: {
                int cat, itemNum;
                cout << "Select category: 1. Savoury 2. Sweets 3. Beverages: ";
                cin >> cat;
                cout << "Enter item number: ";
                cin >> itemNum;
                if (cat == 1 && itemNum <= (int)menu.savoury.size())
                    student.cart.addItem(menu.savoury[itemNum - 1]);
                else if (cat == 2 && itemNum <= (int)menu.sweets.size())
                    student.cart.addItem(menu.sweets[itemNum - 1]);
                else if (cat == 3 && itemNum <= (int)menu.beverages.size())
                    student.cart.addItem(menu.beverages[itemNum - 1]);
                else
                    cout << "❌ Invalid choice.\n";
                break;
            }
            case 3: {
                student.cart.displayCart();
                int removeIndex;
                cout << "Enter item number to remove: ";
                cin >> removeIndex;
                student.cart.removeItem(removeIndex - 1);
                break;
            }
            case 4:
                student.cart.displayCart();
                break;
            case 5:
                student.checkout();
                break;
            case 0:
                cout << "Logging out...\n";
                break;
            default:
                cout << "❌ Invalid choice.\n";
        }

    } while (choice != 0);

    return 0;
}
