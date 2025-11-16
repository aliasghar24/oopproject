#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <regex>
using namespace std;

const string FILE_PATH = "Students.txt";

class Item {
public:
    string name;
    double price;

    Item(string n = "", double p = 0) : name(n), price(p) {}
};

class Category {
public:
    string categoryName;
    vector<Item> items;

    Category(string name = "") : categoryName(name) {}

    void addItem(Item item) {
        items.push_back(item);
    }

    void displayCategory() {
        cout << "\n" << categoryName << ":\n";
        for (size_t i = 0; i < items.size(); i++)
            cout << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price << "\n";
    }
};

class Menu {
public:
    vector<Category> categories;

    Menu() {
        Category savoury("Savoury");
        savoury.addItem(Item("Sandwich", 350));
        savoury.addItem(Item("Burger", 400));
        savoury.addItem(Item("Pizza Slice", 450));
        
        Category sweets("Sweets");
        sweets.addItem(Item("Cookie", 250));
        sweets.addItem(Item("Brownie", 300));
        sweets.addItem(Item("Donut", 200));
        
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
        cout << "\nMenu Categories:\n";
        for (size_t i = 0; i < categories.size(); i++)
            cout << i + 1 << ". " << categories[i].categoryName << "\n";
    }

    void displayCategoryMenu(int categoryIndex) {
        if (categoryIndex >= 0 && categoryIndex < (int)categories.size()) {
            categories[categoryIndex].displayCategory();
        }
    }
};

class ShoppingCart {
public:
    vector<Item> items;

    void addItem(Item item) {
        items.push_back(item);
        cout << "Added " << item.name << " to cart.\n";
    }

    void removeItem(int index) {
        if (index < 0 || index >= (int)items.size()) {
            cout << "Invalid item index.\n";
            return;
        }
        cout << "Removed " << items[index].name << " from cart.\n";
        items.erase(items.begin() + index);
    }

    double getTotal() {
        double total = 0;
        for (auto &item : items)
            total += item.price;
        return total;
    }

    void displayCart() {
        cout << "\nShopping Cart:\n";
        if (items.empty()) {
            cout << "Cart is empty.\n";
            return;
        }
        for (size_t i = 0; i < items.size(); i++)
            cout << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price << "\n";
        cout << "Total: Rs. " << getTotal() << "\n";
    }

    void clearCart() {
        items.clear();
    }

    bool isEmpty() {
        return items.empty();
    }
};

class Order {
public:
    string orderID;
    vector<Item> items;
    double totalAmount;
    time_t orderTime;
    int readyTimeSeconds;
    string status;
    string paymentMethod;
    string phoneNumber;

    Order(string id, vector<Item> cartItems, double total, string method, string phone) 
        : orderID(id), items(cartItems), totalAmount(total), readyTimeSeconds(1200), 
          status("Preparing"), paymentMethod(method), phoneNumber(phone) {
        orderTime = time(0);
    }

    int getTimeRemaining() {
        time_t now = time(0);
        int elapsed = (int)difftime(now, orderTime);
        int remaining = readyTimeSeconds - elapsed;
        return remaining > 0 ? remaining : 0;
    }

    void updateStatus() {
        int remaining = getTimeRemaining();
        if (remaining <= 0) {
            status = "Ready for Pickup";
        }
    }

    void displayOrder() {
        cout << "\nOrder ID: " << orderID;
        cout << "\nItems:\n";
        for (size_t i = 0; i < items.size(); i++)
            cout << "  " << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price << "\n";
        cout << "Total: Rs. " << totalAmount << "\n";
        cout << "Payment Method: " << paymentMethod << "\n";
        cout << "Phone: " << phoneNumber << "\n";
        
        updateStatus();
        cout << "Status: " << status << "\n";
        
        if (status == "Preparing") {
            int remaining = getTimeRemaining();
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            cout << "Time Remaining: " << setfill('0') << setw(2) << minutes << ":" 
                 << setw(2) << seconds << "\n";
        }
    }

    void displayOrderCompact() {
        cout << "\nOrder ID: " << orderID << " | Total: Rs. " << totalAmount;
        updateStatus();
        cout << " | Status: " << status;
        
        if (status == "Preparing") {
            int remaining = getTimeRemaining();
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            cout << " | Time: " << setfill('0') << setw(2) << minutes << ":" << setw(2) << seconds;
        }
        cout << "\n";
    }
};

class OrderHistory {
private:
    vector<Order> orders;
    int orderCounter;

public:
    OrderHistory() : orderCounter(1000) {}

    string generateOrderID() {
        orderCounter++;
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char dateStr[15];
        strftime(dateStr, sizeof(dateStr), "%d%m%Y", timeinfo);
        return string(dateStr) + "ORD" + to_string(orderCounter);
    }

    void addOrder(vector<Item> items, double total, string method, string phone) {
        string ordID = generateOrderID();
        Order newOrder(ordID, items, total, method, phone);
        orders.push_back(newOrder);
        cout << "Order placed successfully! Order ID: " << ordID << "\n";
    }

    void displayOngoingOrders() {
        vector<Order> ongoing;
        for (auto &order : orders) {
            if (order.getTimeRemaining() > 0) {
                ongoing.push_back(order);
            }
        }

        if (ongoing.empty()) {
            cout << "No ongoing orders.\n";
            return;
        }

        cout << "\nOngoing Orders:\n";
        for (auto &order : ongoing) {
            order.displayOrder();
            cout << "---\n";
        }
    }

    void displayCompletedOrders() {
        vector<Order> completed;
        for (auto &order : orders) {
            if (order.getTimeRemaining() <= 0) {
                completed.push_back(order);
            }
        }

        if (completed.empty()) {
            cout << "No completed orders.\n";
            return;
        }

        cout << "\nCompleted Orders:\n";
        for (auto &order : completed) {
            order.displayOrderCompact();
        }
    }

    void displayAllOrders() {
        if (orders.empty()) {
            cout << "No orders yet.\n";
            return;
        }

        cout << "\nAll Orders:\n";
        for (auto &order : orders) {
            order.displayOrder();
            cout << "---\n";
        }
    }

    void trackOrder(string ordID) {
        for (auto &order : orders) {
            if (order.orderID == ordID) {
                order.displayOrder();
                return;
            }
        }
        cout << "Order not found.\n";
    }
};

class Balance {
private:
    double totalBalance;

public:
    Balance(double initial = 1000) : totalBalance(initial) {}

    double getBalance() {
        return totalBalance;
    }

    void displayBalance() {
        cout << "Current Balance: Rs. " << fixed << setprecision(2) << totalBalance << "\n";
    }

    bool deductAmount(double amount) {
        if (amount > totalBalance) {
            cout << "Insufficient balance!\n";
            return false;
        }
        totalBalance -= amount;
        return true;
    }

    void addAmount(double amount) {
        totalBalance += amount;
    }

    bool rechargeBalance(double amount) {
        addAmount(amount);
        cout << "Recharged Rs. " << amount << " successfully!\n";
        return true;
    }
};

class Student {
public:
    string id;
    string password;
    string phoneNumber;
    string email;
    Balance balance;
    ShoppingCart cart;
    OrderHistory orderHistory;

    Student() : balance(1000) {}

    void inputDetails() {
        cout << "Enter Student ID: ";
        cin >> id;
        cout << "Enter Password: ";
        cin >> password;
        
        while (true) {
            cout << "Enter Phone Number (11 digits): ";
            cin >> phoneNumber;
            if (isValidPhoneNumber(phoneNumber)) {
                break;
            }
            cout << "Invalid phone number! Please enter 11 digits.\n";
        }
        
        while (true) {
            cout << "Enter Email: ";
            cin >> email;
            if (isValidEmail(email)) {
                break;
            }
            cout << "Invalid email! Please enter a valid email address.\n";
        }
        
        double initialBalance;
        cout << "Enter Initial Balance (Rs.): ";
        cin >> initialBalance;
        balance = Balance(initialBalance);
    }

    void saveToFile() {
        ofstream outFile(FILE_PATH, ios::app);
        if (!outFile) {
            cout << "Error opening file!\n";
            return;
        }
        outFile << id << " " << password << " " << phoneNumber << " " << email << " " << fixed << setprecision(2) << balance.getBalance() << endl;
        outFile.close();
        cout << "Account saved successfully!\n";
    }

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

        string storedID, storedPass, storedPhone, storedEmail;
        double storedBalance;
        while (inFile >> storedID >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if (loginID == storedID && loginPass == storedPass) {
                cout << "Login successful! Welcome, " << loginID << "!\n";
                id = storedID;
                password = storedPass;
                phoneNumber = storedPhone;
                email = storedEmail;
                balance = Balance(storedBalance);
                inFile.close();
                return true;
            }
        }

        inFile.close();
        cout << "Invalid ID or password.\n";
        return false;
    }

    static bool deleteAccount() {
        string delID, delPass;
        cout << "Enter ID or Email to delete: ";
        cin >> delID;
        cout << "Enter Password: ";
        cin >> delPass;

        ifstream inFile(FILE_PATH);
        ofstream outFile("temp.txt");
        
        if (!inFile) {
            cout << "No user data found.\n";
            return false;
        }

        string storedID, storedPass, storedPhone, storedEmail;
        double storedBalance;
        bool found = false;

        while (inFile >> storedID >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if ((delID == storedID || delID == storedEmail) && delPass == storedPass) {
                found = true;
                cout << "Account deleted successfully!\n";
            } else {
                outFile << storedID << " " << storedPass << " " << storedPhone << " " << storedEmail << " " << fixed << setprecision(2) << storedBalance << endl;
            }
        }

        inFile.close();
        outFile.close();

        if (!found) {
            cout << "Invalid ID/Email or password. Account not deleted.\n";
            remove("temp.txt");
            return false;
        }

        remove(FILE_PATH.c_str());
        rename("temp.txt", FILE_PATH.c_str());
        return true;
    }

    bool isValidPhoneNumber(string phone) {
        if (phone.length() != 11) return false;
        for (char c : phone) {
            if (!isdigit(c)) return false;
        }
        return true;
    }

    bool isValidEmail(string email) {
        regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return regex_match(email, emailRegex);
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
            outFile << storedID << " " << storedPass << " " << fixed << setprecision(2) << storedBalance << endl;
        }
        inFile.close();
        outFile.close();
        remove(FILE_PATH.c_str());
        rename("temp.txt", FILE_PATH.c_str());
    }

    void checkout() {
        if (cart.isEmpty()) {
            cout << "Cart is empty! Add items first.\n";
            return;
        }

        double total = cart.getTotal();
        cout << "\nCheckout:\n";
        cout << "Total Amount: Rs. " << fixed << setprecision(2) << total << "\n";
        balance.displayBalance();

        cout << "\nSelect Payment Method:\n";
        cout << "1. Online\n";
        cout << "2. Card\n";
        cout << "3. Cash\n";
        cout << "Enter choice: ";
        
        int paymentChoice;
        while (!(cin >> paymentChoice) || paymentChoice < 1 || paymentChoice > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice. Please enter 1, 2, or 3: ";
        }

        string paymentMethod;
        if (paymentChoice == 1) {
            paymentMethod = "Online";
            if (total > balance.getBalance()) {
                cout << "Insufficient balance!\n";
                return;
            }
        } else if (paymentChoice == 2) {
            paymentMethod = "Card";
        } else {
            paymentMethod = "Cash";
        }

        cout << "\nOrder Summary:\n";
        cout << "Total: Rs. " << total << "\n";
        cout << "Payment Method: " << paymentMethod << "\n";
        cout << "Phone: " << phoneNumber << "\n";
        cout << "Confirm order? (1 for Yes, 0 for No): ";
        
        int confirm;
        while (!(cin >> confirm) || (confirm != 0 && confirm != 1)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid choice. Enter 1 for Yes or 0 for No: ";
        }

        if (confirm == 0) {
            cout << "Order cancelled.\n";
            return;
        }

        if (paymentChoice == 1) {
            balance.deductAmount(total);
            cout << "Online payment processed.\n";
        } else if (paymentChoice == 2) {
            cout << "Please pay with your card at the counter.\n";
        } else {
            cout << "Please pay in cash at the counter.\n";
        }

        orderHistory.addOrder(cart.items, total, paymentMethod, phoneNumber);
        cout << "Your order will be ready in about 20 minutes!\n";
        balance.displayBalance();
        cart.clearCart();
        updateBalanceInFile();
    }
};

int getValidInput(int minVal, int maxVal) {
    int choice;
    while (true) {
        cout << "Enter your choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input! Please enter a number between " << minVal << " and " << maxVal << ".\n";
            continue;
        }
        if (choice < minVal || choice > maxVal) {
            cout << "Invalid choice! Please enter a number between " << minVal << " and " << maxVal << ".\n";
            continue;
        }
        return choice;
    }
}

int main() {
    Menu menu;
    Student student;
    int choice;

    cout << "GrabNob Bakery System\n";
    cout << "Your Campus Food Ordering App\n\n";
    
    cout << "1. Register New Account\n";
    cout << "2. Login\n";
    cout << "3. Delete Account\n";
    choice = getValidInput(1, 3);

    if (choice == 1) {
        cout << "\nRegistration:\n";
        student.inputDetails();
        student.saveToFile();
        cout << "Please login to continue.\n";
        return 0;
    } else if (choice == 3) {
        Student::deleteAccount();
        return 0;
    } else {
        cout << "\nLogin:\n";
        if (!student.login()) {
            return 0;
        }
    }

    do {
        cout << "\nGrabNob Bakery\n";
        student.balance.displayBalance();
        cout << "\n1. Browse Menu\n";
        cout << "2. View Shopping Cart\n";
        cout << "3. Remove Item from Cart\n";
        cout << "4. Checkout\n";
        cout << "5. View Ongoing Orders\n";
        cout << "6. View Order History\n";
        cout << "7. Recharge Balance\n";
        cout << "0. Logout\n";
        choice = getValidInput(0, 7);

        switch (choice) {
            case 1: {
                menu.displayCategories();
                int cat = getValidInput(0, (int)menu.categories.size());
                if (cat > 0) {
                    menu.displayCategoryMenu(cat - 1);
                    int itemNum = getValidInput(0, (int)menu.categories[cat - 1].items.size());
                    if (itemNum > 0) {
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
                    int removeIndex = getValidInput(0, (int)student.cart.items.size());
                    if (removeIndex > 0) {
                        student.cart.removeItem(removeIndex - 1);
                    }
                }
                break;
            }
            case 4:
                student.checkout();
                break;
            case 5:
                student.orderHistory.displayOngoingOrders();
                break;
            case 6: {
                cout << "\n1. View All Orders\n";
                cout << "2. View Completed Orders\n";
                cout << "3. Track Specific Order\n";
                int histChoice = getValidInput(1, 3);
                if (histChoice == 1) {
                    student.orderHistory.displayAllOrders();
                } else if (histChoice == 2) {
                    student.orderHistory.displayCompletedOrders();
                } else {
                    string ordID;
                    cout << "Enter Order ID: ";
                    cin >> ordID;
                    student.orderHistory.trackOrder(ordID);
                }
                break;
            }
            case 7: {
                double rechargeAmount;
                cout << "Enter amount to recharge (Rs.): ";
                cin >> rechargeAmount;
                if (rechargeAmount > 0) {
                    student.balance.rechargeBalance(rechargeAmount);
                    student.updateBalanceInFile();
                } else {
                    cout << "Invalid amount!\n";
                }
                break;
            }
            case 0:
                cout << "Logging out. Thank you for using GrabNob!\n";
                break;
        }

    } while (choice != 0);

    return 0;
}