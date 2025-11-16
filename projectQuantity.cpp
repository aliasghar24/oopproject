#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <regex>
#include <unordered_map>
using namespace std;

const string FILE_PATH = "Students.txt";

// --------------------- Item ---------------------
class Item {
public:
    string name;
    double price;
    int quantity; // stock (for menu items)

    Item(string n = "", double p = 0, int q = 5) : name(n), price(p), quantity(q) {}
};

// --------------------- CustomSandwich & Ingredient Inventory ---------------------
class IngredientInventory {
public:
    unordered_map<string, int> stock;

    // initialize all ingredient stocks to 10
    void reset() {
        stock.clear();
        // Breads
        stock["Bread_White"] = 10;
        stock["Bread_Brown"] = 10;
        stock["Bread_Multigrain"] = 10;
        // Proteins
        stock["Protein_Chicken"] = 10;
        stock["Protein_Beef"] = 10;
        stock["Protein_Egg"] = 10;
        // Veggies
        stock["Veg_Lettuce"] = 10;
        stock["Veg_Cucumber"] = 10;
        stock["Veg_Olives"] = 10;
        stock["Veg_Onions"] = 10;
        // Sauces
        stock["Sauce_Mayo"] = 10;
        stock["Sauce_Garlic"] = 10;
        stock["Sauce_Chipotle"] = 10;
        // Extras
        stock["Extra_Cheese"] = 10;
    }

    bool hasEnough(const string &key, int qty) {
        auto it = stock.find(key);
        if (it == stock.end()) return false;
        return it->second >= qty;
    }

    // check if a set of keys are available for given qty
    bool hasEnoughAll(const vector<string> &keys, int qty) {
        for (auto &k : keys) {
            if (!hasEnough(k, qty)) return false;
        }
        return true;
    }

    void consume(const string &key, int qty) {
        if (stock.find(key) != stock.end()) stock[key] -= qty;
    }

    void consumeAll(const vector<string> &keys, int qty) {
        for (auto &k : keys) consume(k, qty);
    }

    void restore(const string &key, int qty) {
        if (stock.find(key) != stock.end()) stock[key] += qty;
    }

    void restoreAll(const vector<pair<string,int>> &usages) {
        for (auto &p : usages) restore(p.first, p.second);
    }

    void displayForCustomBuilder() {
        cout << "\n--- Ingredient Availability (per-session) ---\n";
        cout << "Breads: White(" << stock["Bread_White"] << "), Brown(" << stock["Bread_Brown"]
             << "), Multigrain(" << stock["Bread_Multigrain"] << ")\n";
        cout << "Proteins: Chicken(" << stock["Protein_Chicken"] << "), Beef(" << stock["Protein_Beef"]
             << "), Egg(" << stock["Protein_Egg"] << ")\n";
        cout << "Veggies: Lettuce(" << stock["Veg_Lettuce"] << "), Cucumber(" << stock["Veg_Cucumber"]
             << "), Olives(" << stock["Veg_Olives"] << "), Onions(" << stock["Veg_Onions"] << ")\n";
        cout << "Sauces: Mayo(" << stock["Sauce_Mayo"] << "), Garlic(" << stock["Sauce_Garlic"]
             << "), Chipotle(" << stock["Sauce_Chipotle"] << ")\n";
        cout << "Extras: Cheese(" << stock["Extra_Cheese"] << ")\n";
        cout << "--------------------------------------------\n";
    }
};

IngredientInventory ingredientInventory; // global for session

class CustomSandwich {
public:
    string breadKey, proteinKey, vegKey, sauceKey, extraKey;
    string bread, protein, veggies, sauce, extras;
    double price;
    vector<string> chosenIngredientKeys; // keys consumed per 1 sandwich

    CustomSandwich() : price(0) {}

    // Build sandwich — does NOT consume stock yet (consumption happens when user confirms quantity)
    Item buildSandwich() {
        chosenIngredientKeys.clear();
        price = 0;
        cout << "\n====== MAKE YOUR OWN SANDWICH ======\n";

        ingredientInventory.displayForCustomBuilder();

        // Bread
        cout << "Choose Bread:\n1. White (Rs. 50)\n2. Brown (Rs. 60)\n3. Multigrain (Rs. 80)\n";
        int b;
        while (!(cin >> b) || b < 1 || b > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid. Choose 1-3: ";
        }
        if (b == 1) {
            bread = "White"; breadKey = "Bread_White"; price += 50;
        } else if (b == 2) {
            bread = "Brown"; breadKey = "Bread_Brown"; price += 60;
        } else {
            bread = "Multigrain"; breadKey = "Bread_Multigrain"; price += 80;
        }
        chosenIngredientKeys.push_back(breadKey);

        // Protein
        cout << "\nChoose Protein:\n1. Chicken (Rs. 150)\n2. Beef (Rs. 200)\n3. Egg (Rs. 70)\n4. No protein\n";
        int p;
        while (!(cin >> p) || p < 1 || p > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid. Choose 1-4: ";
        }
        if (p == 1) { protein = "Chicken"; proteinKey = "Protein_Chicken"; price += 150; chosenIngredientKeys.push_back(proteinKey); }
        else if (p == 2) { protein = "Beef"; proteinKey = "Protein_Beef"; price += 200; chosenIngredientKeys.push_back(proteinKey); }
        else if (p == 3) { protein = "Egg"; proteinKey = "Protein_Egg"; price += 70; chosenIngredientKeys.push_back(proteinKey); }
        else { protein = "None"; proteinKey = ""; }

        // Veggies
        cout << "\nChoose Veggies:\n1. Lettuce (Rs. 40)\n2. Cucumber (Rs. 50)\n3. Olives (Rs. 30)\n4. Onions (Rs. 20)\n5. No Veggies\n";
        int v;
        while (!(cin >> v) || v < 1 || v > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid. Choose 1-5: ";
        }
        if (v == 1) { veggies = "Lettuce"; vegKey = "Veg_Lettuce"; price += 40; chosenIngredientKeys.push_back(vegKey); }
        else if (v == 2) { veggies = "Cucumber"; vegKey = "Veg_Cucumber"; price += 50; chosenIngredientKeys.push_back(vegKey);}
        else if (v == 3) { veggies = "Olives"; vegKey = "Veg_Olives"; price += 30; chosenIngredientKeys.push_back(vegKey);}
        else if (v == 4) { veggies = "Onions"; vegKey = "Veg_Onions"; price += 20; chosenIngredientKeys.push_back(vegKey);}
        else { veggies = "None"; vegKey = ""; }

        // Sauce
        cout << "\nChoose Sauce (Rs. 20):\n1. Mayo\n2. Garlic\n3. Chipotle\n4. No Sauce\n";
        int s;
        while (!(cin >> s) || s < 1 || s > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid. Choose 1-4: ";
        }
        if (s == 1) { sauce = "Mayo"; sauceKey = "Sauce_Mayo"; price += 20; chosenIngredientKeys.push_back(sauceKey); }
        else if (s == 2) { sauce = "Garlic"; sauceKey = "Sauce_Garlic"; price += 20; chosenIngredientKeys.push_back(sauceKey); }
        else if (s == 3) { sauce = "Chipotle"; sauceKey = "Sauce_Chipotle"; price += 20; chosenIngredientKeys.push_back(sauceKey); }
        else { sauce = "None"; sauceKey = ""; }

        // Extras
        cout << "\nAdd Extra Cheese? (Rs. 50)\n1. Yes\n2. No\n";
        int e;
        while (!(cin >> e) || (e != 1 && e != 2)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid. Choose 1 or 2: ";
        }
        if (e == 1) { extras = "Cheese"; extraKey = "Extra_Cheese"; price += 50; chosenIngredientKeys.push_back(extraKey); }
        else { extras = "None"; extraKey = ""; }

        string name = "Custom Sandwich (" + bread + ", " + protein + ", " + veggies + ", " + sauce + ", " + extras + ")";
        return Item(name, price, /*quantity not used for custom*/ 0);
    }
};

// --------------------- Category & Menu ---------------------
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
        for (size_t i = 0; i < items.size(); i++) {
            cout << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price
                 << " | Stock: " << items[i].quantity;
            if (items[i].quantity == 0) cout << " (OUT OF STOCK)";
            cout << "\n";
        }
    }
};

class Menu {
public:
    vector<Category> categories;

    Menu() {
        Category savoury("Savoury");
        savoury.addItem(Item("Sandwich", 350, 5));
        savoury.addItem(Item("Burger", 400, 5));
        savoury.addItem(Item("Pizza Slice", 450, 5));
        savoury.addItem(Item("Make Your Own Sandwich", 100, 5)); // this is just an option; uses ingredient stock

        Category sweets("Sweets");
        sweets.addItem(Item("Cookie", 250, 5));
        sweets.addItem(Item("Brownie", 300, 5));
        sweets.addItem(Item("Donut", 200, 5));

        Category beverages("Beverages");
        beverages.addItem(Item("Coffee", 400, 5));
        beverages.addItem(Item("Pepsi", 60, 5));
        beverages.addItem(Item("Tea", 150, 5));
        beverages.addItem(Item("Juice", 200, 5));

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

    // reset all menu item stocks to 5
    void resetStock() {
        for (auto &cat : categories)
            for (auto &it : cat.items)
                it.quantity = 5;
    }

    // increase stock for a menu item by name (used when removing from cart)
    void increaseStockByName(const string &itemName, int qty) {
        for (auto &cat : categories)
            for (auto &it : cat.items)
                if (it.name == itemName) {
                    it.quantity += qty;
                    return;
                }
    }

    // find item reference by category index & item index (both 0-based)
    Item* getItemRef(int catIdx, int itemIdx) {
        if (catIdx < 0 || catIdx >= (int)categories.size()) return nullptr;
        if (itemIdx < 0 || itemIdx >= (int)categories[catIdx].items.size()) return nullptr;
        return &categories[catIdx].items[itemIdx];
    }
};

// --------------------- CartItem & ShoppingCart ---------------------
class CartItem {
public:
    string name;
    double price;
    int quantity;
    bool isCustom;
    vector<pair<string,int>> ingredientUsage; // <ingredientKey, qty> if custom

    CartItem(string n = "", double p = 0, int q = 0) : name(n), price(p), quantity(q), isCustom(false) {}
};

class ShoppingCart {
public:
    vector<CartItem> items;

    void addItem(const string &name, double price, int qty, bool isCustom=false, const vector<pair<string,int>> &usage = {}) {
        // If same non-custom item exists, increase quantity
        for (auto &ci : items) {
            if (!ci.isCustom && !isCustom && ci.name == name) {
                ci.quantity += qty;
                cout << "✅ Increased " << ci.name << " quantity in cart by " << qty << ".\n";
                return;
            }
            // For custom sandwiches, we treat each custom composition as unique (could compare ingredientUsage)
            if (ci.isCustom && isCustom && ci.name == name && ci.ingredientUsage == usage) {
                ci.quantity += qty;
                cout << "✅ Increased custom sandwich quantity in cart by " << qty << ".\n";
                return;
            }
        }
        CartItem c(name, price, qty);
        c.isCustom = isCustom;
        c.ingredientUsage = usage;
        items.push_back(c);
        cout << "✅ Added " << name << " x" << qty << " to cart.\n";
    }

    bool removeItem(int index, CartItem &removed) {
        if (index < 0 || index >= (int)items.size()) {
            cout << "Invalid index.\n";
            return false;
        }
        removed = items[index];
        cout << "❌ Removed " << removed.name << " x" << removed.quantity << " from cart.\n";
        items.erase(items.begin() + index);
        return true;
    }

    double getTotal() {
        double total = 0;
        for (auto &item : items)
            total += item.price * item.quantity;
        return total;
    }

    void displayCart() {
        cout << "\nShopping Cart:\n";
        if (items.empty()) {
            cout << "Cart is empty.\n";
            return;
        }
        for (size_t i = 0; i < items.size(); i++) {
            cout << i + 1 << ". " << items[i].name << " x" << items[i].quantity
                 << " - Rs. " << items[i].price << " each"
                 << " | Subtotal: Rs. " << items[i].price * items[i].quantity << "\n";
            if (items[i].isCustom) {
                cout << "   Ingredients consumed per sandwich:\n";
                for (auto &p : items[i].ingredientUsage)
                    cout << "    - " << p.first << "\n";
            }
        }
        cout << "Total: Rs. " << getTotal() << "\n";
    }

    void clearCart() {
        items.clear();
    }

    bool isEmpty() {
        return items.empty();
    }
};

// --------------------- Order, OrderHistory (unchanged) ---------------------
class Order {
public:
    string orderID;
    vector<CartItem> items;
    double totalAmount;
    time_t orderTime;
    int readyTimeSeconds;
    string status;
    string paymentMethod;
    string phoneNumber;

    Order(string id, const vector<CartItem> &cartItems, double total, string method, string phone)
        : orderID(id), totalAmount(total), readyTimeSeconds(1200), status("Preparing"), paymentMethod(method), phoneNumber(phone) {
        orderTime = time(0);
        items = cartItems;
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
            cout << "  " << i + 1 << ". " << items[i].name << " x" << items[i].quantity << " - Rs. " << items[i].price << "\n";
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

    void addOrder(const vector<CartItem> &items, double total, string method, string phone) {
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

// --------------------- Balance ---------------------
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

// --------------------- Student ---------------------
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
            if (isValidPhoneNumber(phoneNumber)) break;
            cout << "Invalid phone number! Please enter 11 digits.\n";
        }

        while (true) {
            cout << "Enter Email: ";
            cin >> email;
            if (isValidEmail(email)) break;
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
        for (char c : phone) if (!isdigit(c)) return false;
        return true;
    }

    bool isValidEmail(string email) {
        regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return regex_match(email, emailRegex);
    }

    void updateBalanceInFile() {
        ifstream inFile(FILE_PATH);
        ofstream outFile("temp.txt");
        string storedID, storedPass, storedPhone, storedEmail;
        double storedBalance;
        while (inFile >> storedID >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if (storedID == id) {
                storedBalance = balance.getBalance();
            }
            outFile << storedID << " " << storedPass << " " << storedPhone << " " << storedEmail << " " << fixed << setprecision(2) << storedBalance << endl;
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
            balance.deductAmount(total);
            cout << "Online payment processed.\n";
        } else if (paymentChoice == 2) {
            paymentMethod = "Card";
            cout << "Please pay with your card at the counter.\n";
        } else {
            paymentMethod = "Cash";
            cout << "Please pay in cash at the counter.\n";
        }

        // place order
        orderHistory.addOrder(cart.items, total, paymentMethod, phoneNumber);
        cout << "Your order will be ready in about 20 minutes!\n";
        balance.displayBalance();
        // cart cleared after order
        cart.clearCart();
        updateBalanceInFile();
    }
};

// --------------------- Utility ---------------------
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

// --------------------- MAIN ---------------------
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
        // Reset menu stock AND ingredient stock for this session
        menu.resetStock();
        ingredientInventory.reset();
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
                cout << "\nSelect category (0 to go back):\n";
                int cat = getValidInput(0, (int)menu.categories.size());
                if (cat > 0) {
                    menu.displayCategoryMenu(cat - 1);
                    cout << "\nEnter item number to add to cart (0 to go back):\n";
                    int itemNum = getValidInput(0, (int)menu.categories[cat - 1].items.size());
                    if (itemNum > 0) {
                        Item *selected = menu.getItemRef(cat - 1, itemNum - 1);
                        if (!selected) break;

                        // custom sandwich option
                        if (selected->name == "Make Your Own Sandwich") {
                            CustomSandwich cs;
                            Item customSample = cs.buildSandwich(); // chosenIngredientKeys stored in cs
                            // ask how many of this custom sandwich to add
                            cout << "Enter quantity of this custom sandwich to add: ";
                            int qty;
                            while (!(cin >> qty) || qty <= 0) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Invalid quantity. Enter positive integer: ";
                            }

                            // check ingredient availability
                            if (!ingredientInventory.hasEnoughAll(cs.chosenIngredientKeys, qty)) {
                                cout << "❌ Not enough ingredient stock for " << qty << " sandwich(es).\n";
                                // show available quantities for the missing ones
                                for (auto &k : cs.chosenIngredientKeys) {
                                    cout << "   " << k << ": " << ingredientInventory.stock[k] << " available\n";
                                }
                            } else {
                                // consume ingredients
                                ingredientInventory.consumeAll(cs.chosenIngredientKeys, qty);
                                // prepare ingredientUsage vector for cart item restoration if removed
                                vector<pair<string,int>> usage;
                                for (auto &k : cs.chosenIngredientKeys) usage.push_back({k, qty});
                                // add to cart as custom
                                student.cart.addItem(customSample.name, customSample.price, qty, true, usage);
                            }
                        } else {
                            // normal menu item -> ask quantity, check stock
                            cout << "Enter quantity to add: ";
                            int qty;
                            while (!(cin >> qty) || qty <= 0) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Invalid quantity. Enter positive integer: ";
                            }
                            if (qty > selected->quantity) {
                                cout << "❌ Not enough stock. Available: " << selected->quantity << "\n";
                            } else {
                                selected->quantity -= qty;
                                student.cart.addItem(selected->name, selected->price, qty, false, {});
                            }
                        }
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
                    cout << "\nEnter item number to remove (0 to cancel):\n";
                    int removeIndex = getValidInput(0, (int)student.cart.items.size());
                    if (removeIndex > 0) {
                        CartItem &itemToRemove = student.cart.items[removeIndex - 1];
                        cout << "You have " << itemToRemove.quantity << " of " << itemToRemove.name << " in your cart.\n";
                        cout << "Enter quantity to remove (1-" << itemToRemove.quantity << "): ";
                        int qtyToRemove;
                        while (!(cin >> qtyToRemove) || qtyToRemove < 1 || qtyToRemove > itemToRemove.quantity) {
                            cin.clear();
                            cin.ignore(10000, '\n');
                            cout << "Invalid quantity. Enter a number between 1 and " << itemToRemove.quantity << ": ";
                        }

                        // restore stock
                        if (itemToRemove.isCustom) {
                            // restore each ingredient usage proportionally
                            vector<pair<string,int>> restoreUsage;
                            for (auto &p : itemToRemove.ingredientUsage) {
                                restoreUsage.push_back({p.first, qtyToRemove});
                            }
                            ingredientInventory.restoreAll(restoreUsage);
                        } else {
                            menu.increaseStockByName(itemToRemove.name, qtyToRemove);
                        }

                        itemToRemove.quantity -= qtyToRemove;
                        cout << "❌ Removed " << qtyToRemove << " of " << itemToRemove.name << " from cart.\n";

                        // if quantity reaches 0, erase the item completely
                        if (itemToRemove.quantity == 0) {
                            student.cart.items.erase(student.cart.items.begin() + (removeIndex - 1));
                        }
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
                if (histChoice == 1) student.orderHistory.displayAllOrders();
                else if (histChoice == 2) student.orderHistory.displayCompletedOrders();
                else {
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
