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
const string ORDERS_FILE = "Orders.txt";

// --------------------- PaymentMethod (BASE CLASS) ---------------------
class PaymentMethod {
public:
    virtual ~PaymentMethod() {}
    
    virtual bool processPayment(double amount, double &balance) = 0;
    virtual string getMethodName() = 0;
    virtual void displayPaymentInfo() = 0;
};

// --------------------- OnlinePayment (CHILD CLASS) ---------------------
class OnlinePayment : public PaymentMethod {
public:
    bool processPayment(double amount, double &balance) override {
        if (amount > balance) {
            std::cout << "Insufficient balance!\n";
            return false;
        }
        balance -= amount;
        std::cout << "Online payment of Rs. " << fixed << setprecision(2) << amount << " processed successfully!\n";
        return true;
    }
    
    string getMethodName() override {
        return "Online";
    }
    
    void displayPaymentInfo() override {
        std::cout << "Payment Method: Online (Deducted from wallet)\n";
    }
};

// --------------------- CardPayment (CHILD CLASS) ---------------------
class CardPayment : public PaymentMethod {
public:
    bool processPayment(double amount, double &balance) override {
        std::cout << "Processing card payment of Rs. " << fixed << setprecision(2) << amount << "...\n";
        std::cout << "Please pay with your card at the counter.\n";
        return true;
    }
    
    string getMethodName() override {
        return "Card";
    }
    
    void displayPaymentInfo() override {
        std::cout << "Payment Method: Card (Pay at counter)\n";
    }
};

// --------------------- CashPayment (CHILD CLASS) ---------------------
class CashPayment : public PaymentMethod {
public:
    bool processPayment(double amount, double &balance) override {
        std::cout << "Cash payment of Rs. " << fixed << setprecision(2) << amount << " registered.\n";
        std::cout << "Please pay in cash at the counter.\n";
        return true;
    }
    
    string getMethodName() override {
        return "Cash";
    }
    
    void displayPaymentInfo() override {
        std::cout << "Payment Method: Cash (Pay at counter)\n";
    }
};

// --------------------- Item ---------------------
class Item {
public:
    string name;
    double price;
    int quantity;

    Item(string n = "", double p = 0, int q = 5) : name(n), price(p), quantity(q) {}
};

// --------------------- IngredientInventory ---------------------
class IngredientInventory {
public:
    unordered_map<string, int> stock;

    void reset() {
        stock.clear();
        stock["Bread_White"] = 10;
        stock["Bread_Brown"] = 10;
        stock["Bread_Multigrain"] = 10;
        stock["Protein_Chicken"] = 10;
        stock["Protein_Beef"] = 10;
        stock["Protein_Egg"] = 10;
        stock["Veg_Lettuce"] = 10;
        stock["Veg_Cucumber"] = 10;
        stock["Veg_Olives"] = 10;
        stock["Veg_Onions"] = 10;
        stock["Sauce_Mayo"] = 10;
        stock["Sauce_Garlic"] = 10;
        stock["Sauce_Chipotle"] = 10;
        stock["Extra_Cheese"] = 10;
    }

    bool hasEnough(const string &key, int qty) {
        auto it = stock.find(key);
        if (it == stock.end()) return false;
        return it->second >= qty;
    }

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
        std::cout << "\n--- Ingredient Availability (per-session) ---\n";
        std::cout << "Breads: White(" << stock["Bread_White"] << "), Brown(" << stock["Bread_Brown"]
             << "), Multigrain(" << stock["Bread_Multigrain"] << ")\n";
        std::cout << "Proteins: Chicken(" << stock["Protein_Chicken"] << "), Beef(" << stock["Protein_Beef"]
             << "), Egg(" << stock["Protein_Egg"] << ")\n";
        std::cout << "Veggies: Lettuce(" << stock["Veg_Lettuce"] << "), Cucumber(" << stock["Veg_Cucumber"]
             << "), Olives(" << stock["Veg_Olives"] << "), Onions(" << stock["Veg_Onions"] << ")\n";
        std::cout << "Sauces: Mayo(" << stock["Sauce_Mayo"] << "), Garlic(" << stock["Sauce_Garlic"]
             << "), Chipotle(" << stock["Sauce_Chipotle"] << ")\n";
        std::cout << "Extras: Cheese(" << stock["Extra_Cheese"] << ")\n";
        std::cout << "--------------------------------------------\n";
    }
    void saveToFile() {
    ofstream outFile("IngredientStock.txt");
    if (!outFile) return;
    
    for (auto &pair : stock) {
        outFile << pair.first << "||" << pair.second << "\n";
    }
    outFile.close();
}

void loadFromFile() {
    ifstream inFile("IngredientStock.txt");
    if (!inFile) {
        reset();
        saveToFile();
        return;
    }
    
    stock.clear();
    string line;
    while (getline(inFile, line)) {
        size_t pos = line.find("||");
        if (pos != string::npos) {
            string key = line.substr(0, pos);
            int quantity = stoi(line.substr(pos + 2));
            stock[key] = quantity;
        }
    }
    inFile.close();
}
};

IngredientInventory ingredientInventory;

// --------------------- CustomSandwich ---------------------
class CustomSandwich {
public:
    string breadKey, proteinKey, vegKey, sauceKey, extraKey;
    string bread, protein, veggies, sauce, extras;
    double price;
    vector<string> chosenIngredientKeys;

    CustomSandwich() : price(0) {}

    Item buildSandwich() {
        chosenIngredientKeys.clear();
        price = 0;
        std::cout << "\n====== MAKE YOUR OWN SANDWICH ======\n";

        ingredientInventory.displayForCustomBuilder();

        // Bread
        std::cout << "Choose Bread:\n1. White (Rs. 50)\n2. Brown (Rs. 60)\n3. Multigrain (Rs. 80)\n";
        int b;
        while (!(cin >> b) || b < 1 || b > 3) {
            cin.clear();
            cin.ignore(10000, '\n');
            std::cout << "Invalid. Choose 1-3: ";
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
        std::cout << "\nChoose Protein:\n1. Chicken (Rs. 150)\n2. Beef (Rs. 200)\n3. Egg (Rs. 70)\n4. No protein\n";
        int p;
        while (!(cin >> p) || p < 1 || p > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            std::cout << "Invalid. Choose 1-4: ";
        }
        if (p == 1) { protein = "Chicken"; proteinKey = "Protein_Chicken"; price += 150; chosenIngredientKeys.push_back(proteinKey); }
        else if (p == 2) { protein = "Beef"; proteinKey = "Protein_Beef"; price += 200; chosenIngredientKeys.push_back(proteinKey); }
        else if (p == 3) { protein = "Egg"; proteinKey = "Protein_Egg"; price += 70; chosenIngredientKeys.push_back(proteinKey); }
        else { protein = "None"; proteinKey = ""; }

        // Veggies
        std::cout << "\nChoose Veggies:\n1. Lettuce (Rs. 40)\n2. Cucumber (Rs. 50)\n3. Olives (Rs. 30)\n4. Onions (Rs. 20)\n5. No Veggies\n";
        int v;
        while (!(cin >> v) || v < 1 || v > 5) {
            cin.clear();
            cin.ignore(10000, '\n');
            std::cout << "Invalid. Choose 1-5: ";
        }
        if (v == 1) { veggies = "Lettuce"; vegKey = "Veg_Lettuce"; price += 40; chosenIngredientKeys.push_back(vegKey); }
        else if (v == 2) { veggies = "Cucumber"; vegKey = "Veg_Cucumber"; price += 50; chosenIngredientKeys.push_back(vegKey);}
        else if (v == 3) { veggies = "Olives"; vegKey = "Veg_Olives"; price += 30; chosenIngredientKeys.push_back(vegKey);}
        else if (v == 4) { veggies = "Onions"; vegKey = "Veg_Onions"; price += 20; chosenIngredientKeys.push_back(vegKey);}
        else { veggies = "None"; vegKey = ""; }

        // Sauce
        std::cout << "\nChoose Sauce (Rs. 20):\n1. Mayo\n2. Garlic\n3. Chipotle\n4. No Sauce\n";
        int s;
        while (!(cin >> s) || s < 1 || s > 4) {
            cin.clear();
            cin.ignore(10000, '\n');
            std::cout << "Invalid. Choose 1-4: ";
        }
        if (s == 1) { sauce = "Mayo"; sauceKey = "Sauce_Mayo"; price += 20; chosenIngredientKeys.push_back(sauceKey); }
        else if (s == 2) { sauce = "Garlic"; sauceKey = "Sauce_Garlic"; price += 20; chosenIngredientKeys.push_back(sauceKey); }
        else if (s == 3) { sauce = "Chipotle"; sauceKey = "Sauce_Chipotle"; price += 20; chosenIngredientKeys.push_back(sauceKey); }
        else { sauce = "None"; sauceKey = ""; }

        // Extras
        std::cout << "\nAdd Extra Cheese? (Rs. 50)\n1. Yes\n2. No\n";
        int e;
        while (!(cin >> e) || (e != 1 && e != 2)) {
            cin.clear();
            cin.ignore(10000, '\n');
            std::cout << "Invalid. Choose 1 or 2: ";
        }
        if (e == 1) { extras = "Cheese"; extraKey = "Extra_Cheese"; price += 50; chosenIngredientKeys.push_back(extraKey); }
        else { extras = "None"; extraKey = ""; }

        string name = "Custom Sandwich (" + bread + ", " + protein + ", " + veggies + ", " + sauce + ", " + extras + ")";
        return Item(name, price, 0);
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
        std::cout << "\n" << categoryName << ":\n";
        for (size_t i = 0; i < items.size(); i++) {
            std::cout << i + 1 << ". " << items[i].name << " - Rs. " << items[i].price
                 << " | Stock: " << items[i].quantity;
            if (items[i].quantity == 0) std::cout << " (OUT OF STOCK)";
            std::cout << "\n";
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
        savoury.addItem(Item("Make Your Own Sandwich", 100, 5));

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
        std::cout << "\nMenu Categories:\n";
        for (size_t i = 0; i < categories.size(); i++)
            std::cout << i + 1 << ". " << categories[i].categoryName << "\n";
    }

    void displayCategoryMenu(int categoryIndex) {
        if (categoryIndex >= 0 && categoryIndex < (int)categories.size()) {
            categories[categoryIndex].displayCategory();
        }
    }

    void resetStock() {
        for (auto &cat : categories)
            for (auto &it : cat.items)
                it.quantity = 5;
    }

    void increaseStockByName(const string &itemName, int qty) {
        for (auto &cat : categories)
            for (auto &it : cat.items)
                if (it.name == itemName) {
                    it.quantity += qty;
                    return;
                }
    }

    Item* getItemRef(int catIdx, int itemIdx) {
        if (catIdx < 0 || catIdx >= (int)categories.size()) return nullptr;
        if (itemIdx < 0 || itemIdx >= (int)categories[catIdx].items.size()) return nullptr;
        return &categories[catIdx].items[itemIdx];
    }
    void saveStockToFile() {
    ofstream outFile("MenuStock.txt");
    if (!outFile) return;
    
    for (auto &cat : categories) {
        for (auto &item : cat.items) {
            outFile << item.name << "||" << item.quantity << "\n";
        }
    }
    outFile.close();
}

void loadStockFromFile() {
    ifstream inFile("MenuStock.txt");
    if (!inFile) {
        // If file doesn't exist, initialize with default stock
        resetStock();
        saveStockToFile();
        return;
    }
    
    string line;
    while (getline(inFile, line)) {
        size_t pos = line.find("||");
        if (pos != string::npos) {
            string itemName = line.substr(0, pos);
            int quantity = stoi(line.substr(pos + 2));
            
            // Update quantity for matching item
            for (auto &cat : categories) {
                for (auto &item : cat.items) {
                    if (item.name == itemName) {
                        item.quantity = quantity;
                    }
                }
            }
        }
    }
    inFile.close();
}
};

// --------------------- CartItem & ShoppingCart ---------------------
class CartItem {
public:
    string name;
    double price;
    int quantity;
    bool isCustom;
    vector<pair<string,int>> ingredientUsage;

    CartItem(string n = "", double p = 0, int q = 0) : name(n), price(p), quantity(q), isCustom(false) {}
};

class ShoppingCart {
public:
    vector<CartItem> items;

    void addItem(const string &name, double price, int qty, bool isCustom=false, const vector<pair<string,int>> &usage = {}) {
        for (auto &ci : items) {
            if (!ci.isCustom && !isCustom && ci.name == name) {
                ci.quantity += qty;
                std::cout << "Increased " << ci.name << " quantity in cart by " << qty << ".\n";
                return;
            }
            if (ci.isCustom && isCustom && ci.name == name && ci.ingredientUsage == usage) {
                ci.quantity += qty;
                std::cout << "Increased custom sandwich quantity in cart by " << qty << ".\n";
                return;
            }
        }
        CartItem c(name, price, qty);
        c.isCustom = isCustom;
        c.ingredientUsage = usage;
        items.push_back(c);
        std::cout << "Added " << name << " x" << qty << " to cart.\n";
    }

    bool removeItem(int index, CartItem &removed) {
        if (index < 0 || index >= (int)items.size()) {
            std::cout << "Invalid index.\n";
            return false;
        }
        removed = items[index];
        std::cout << "Removed " << removed.name << " x" << removed.quantity << " from cart.\n";
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
        std::cout << "\nShopping Cart:\n";
        if (items.empty()) {
            std::cout << "Cart is empty.\n";
            return;
        }
        for (size_t i = 0; i < items.size(); i++) {
            std::cout << i + 1 << ". " << items[i].name << " x" << items[i].quantity
                 << " - Rs. " << items[i].price << " each"
                 << " | Subtotal: Rs. " << items[i].price * items[i].quantity << "\n";
            if (items[i].isCustom) {
                std::cout << "   Ingredients:\n";
                for (auto &p : items[i].ingredientUsage)
                    std::cout << "    - " << p.first << "\n";
            }
        }
        std::cout << "Total: Rs. " << getTotal() << "\n";
    }

    void clearCart() {
        items.clear();
    }

    bool isEmpty() {
        return items.empty();
    }
};

// --------------------- Order & OrderHistory ---------------------
class Order {
public:
    string orderID;
    string username;
    string orderDate;
    vector<Item> items;
    vector<int> quantities;
    double totalAmount;
    time_t orderTime;
    int readyTimeSeconds;
    string status;
    string paymentMethod;
    string phoneNumber;

    Order(string id, string user, string date, vector<Item> cartItems, vector<int> qtys, double total, string method, string phone) 
        : orderID(id), username(user), orderDate(date), items(cartItems), quantities(qtys), totalAmount(total), readyTimeSeconds(60), 
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
        std::cout << "\nOrder ID: " << orderID;
        std::cout << "\nCustomer: " << username;
        std::cout << "\nDate: " << orderDate;
        std::cout << "\nItems:\n";
        for (size_t i = 0; i < items.size(); i++)
            std::cout << "  " << i + 1 << ". " << items[i].name << " x" << quantities[i] << " - Rs. " << items[i].price << " each | Subtotal: Rs. " << (items[i].price * quantities[i]) << "\n";
        std::cout << "Total: Rs. " << totalAmount << "\n";
        std::cout << "Payment Method: " << paymentMethod << "\n";
        std::cout << "Phone: " << phoneNumber << "\n";
        
        updateStatus();
        std::cout << "Status: " << status << "\n";
        
        if (status == "Preparing") {
            int remaining = getTimeRemaining();
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            std::cout << "Time Remaining: "<<setfill('0')<<setw(2)<<minutes<< ":" << setw(2) << seconds << "\n";
        }
    }

    void displayOrderCompact() {
        std::cout << "\nOrder ID: " << orderID << " | Customer: " << username << " | Date: " << orderDate << " | Total: Rs. " << totalAmount;
        updateStatus();
        std::cout << " | Status: " << status << endl;
        
        if (status == "Preparing") {
            int remaining = getTimeRemaining();
            int minutes = remaining / 60;
            int seconds = remaining % 60;
            std::cout << "Time Remaining: "<<setfill('0')<<setw(2)<<minutes<< ":" << setw(2) << seconds << "\n";
        }
    }
    
};

class OrderHistory {
private:
    vector<Order> orders;
    int orderCounter;

public:
    OrderHistory() : orderCounter(0) {
        loadOrderCounterFromFile();
        loadOrdersFromFile();
    }

    void loadOrderCounterFromFile() {
        ifstream inFile(ORDERS_FILE);
        if (!inFile) {
            return;
        }

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            size_t pos = 0;
            string orderID = line.substr(0, line.find("||"));
            int orderNum = stoi(orderID.substr(11));
            if (orderNum > orderCounter) {
                orderCounter = orderNum;
            }
        }
        inFile.close();
    }

    string generateOrderID() {
        orderCounter++;
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char dateStr[15];
        strftime(dateStr, sizeof(dateStr), "%d%m%Y", timeinfo);
        return string(dateStr) + "ORD" + to_string(orderCounter);
    }

    string getCurrentDate() {
        time_t now = time(0);
        tm* timeinfo = localtime(&now);
        char dateStr[20];
        strftime(dateStr, sizeof(dateStr), "%d-%m-%Y %H:%M:%S", timeinfo);
        return string(dateStr);
    }

    void addOrder(vector<Item> items, vector<int> quantities, double total, string method, string phone, string username) {
        string ordID = generateOrderID();
        string date = getCurrentDate();
        Order newOrder(ordID, username, date, items, quantities, total, method, phone);
        orders.push_back(newOrder);
        saveOrderToFile(newOrder);
        std::cout << "Order placed successfully! Order ID: " << ordID << "\n";
    }

    void saveOrderToFile(Order &order) {
        ofstream outFile(ORDERS_FILE, ios::app);
        if (!outFile) {
            std::cout << "Error saving order!\n";
            return;
        }
        outFile << order.orderID << "||" << order.username << "||" << order.orderDate << "||" 
                << order.totalAmount << "||" << order.paymentMethod << "||" 
                << order.phoneNumber << "||" << order.orderTime << "||";
        for (size_t i = 0; i < order.items.size(); i++) {
            string itemName = order.items[i].name;
            for (char &c : itemName) {
                if (c == '|' || c == ':' || c == ',') c = ' ';
            }
            outFile << itemName << ":" << order.items[i].price << ":" << order.quantities[i];
            if (i < order.items.size() - 1) outFile << ",";
        }
        outFile << "\n";
        outFile.close();
    }

    void loadOrdersFromFile() {
        ifstream inFile(ORDERS_FILE);
        if (!inFile) {
            return;
        }

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            
            try {
                size_t pos = 0;
                
                string orderID = line.substr(0, line.find("||"));
                pos = line.find("||") + 2;
                
                string username = line.substr(pos, line.find("||", pos) - pos);
                pos = line.find("||", pos) + 2;
                
                string orderDate = line.substr(pos, line.find("||", pos) - pos);
                pos = line.find("||", pos) + 2;
                
                double totalAmount = stod(line.substr(pos, line.find("||", pos) - pos));
                pos = line.find("||", pos) + 2;
                
                string paymentMethod = line.substr(pos, line.find("||", pos) - pos);
                pos = line.find("||", pos) + 2;
                
                string phoneNumber = line.substr(pos, line.find("||", pos) - pos);
                pos = line.find("||", pos) + 2;
                
                time_t orderTime = stol(line.substr(pos, line.find("||", pos) - pos));
                pos = line.find("||", pos) + 2;
                
                string itemsStr = line.substr(pos);
                vector<Item> items;
                vector<int> quantities;
                
                size_t itemPos = 0;
                while (itemPos < itemsStr.length()) {
                    size_t commaPos = itemsStr.find(",", itemPos);
                    if (commaPos == string::npos) commaPos = itemsStr.length();
                    
                    string itemStr = itemsStr.substr(itemPos, commaPos - itemPos);
                    size_t colonPos = itemStr.rfind(":");
                    
                    if (colonPos != string::npos) {
                        size_t secondColonPos = itemStr.rfind(":", colonPos - 1);
                        if (secondColonPos != string::npos) {
                            string itemName = itemStr.substr(0, secondColonPos);
                            double itemPrice = stod(itemStr.substr(secondColonPos + 1, colonPos - secondColonPos - 1));
                            int itemQty = stoi(itemStr.substr(colonPos + 1));
                            items.push_back(Item(itemName, itemPrice));
                            quantities.push_back(itemQty);
                        }
                    }
                    
                    itemPos = commaPos + 1;
                }
                
                Order loadedOrder(orderID, username, orderDate, items, quantities, totalAmount, paymentMethod, phoneNumber);
                loadedOrder.orderTime = orderTime;
                orders.push_back(loadedOrder);
            } catch (const exception &e) {
                std::cout << "Warning: Could not load order from file: " << e.what() << "\n";
                continue;
            }
        }
        inFile.close();
    }

    void displayOngoingOrders() {
        vector<Order> ongoing;
        for (auto &order : orders) {
            if (order.getTimeRemaining() > 0) {
                ongoing.push_back(order);
            }
        }

        if (ongoing.empty()) {
            std::cout << "No ongoing orders.\n";
            return;
        }

        std::cout << "\nOngoing Orders:\n";
        for (auto &order : ongoing) {
            order.displayOrder();
            std::cout << "---\n";
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
            std::cout << "No completed orders.\n";
            return;
        }

        std::cout << "\nCompleted Orders:\n";
        for (auto &order : completed) {
            order.displayOrderCompact();
        }
    }

    void displayAllOrders() {
        displayOngoingOrders();
        displayCompletedOrders();
    }

    void trackOrder(string ordID) {
        for (auto &order : orders) {
            if (order.orderID == ordID) {
                order.displayOrder();
                return;
            }
        }
        std::cout << "Order not found.\n";
    }
};

// --------------------- Balance ---------------------
class Balance {
private:
    double totalBalance;

public:
    Balance(double initial = 0) : totalBalance(initial) {}

    double getBalance() {
        return totalBalance;
    }

    void displayBalance() {
        std::cout << "Current Balance: Rs. " << fixed << setprecision(2) << totalBalance << "\n";
    }

    bool deductAmount(double amount) {
        if (amount > totalBalance) {
            std::cout << "Insufficient balance!\n";
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
        std::cout << "Recharged Rs. " << amount << " successfully!\n";
        return true;
    }
};

// --------------------- Student ---------------------
class Student {
public:
    string id;
    string username;
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
        cout << "Enter Username: ";
        cin >> username;
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
        // ofstream = it is used to write to files
        // ios::app = means append mode. do NOT erase old data, just add new data at the bottom of the file.
        ofstream outFile(FILE_PATH, ios::app);
        if (!outFile) {
            cout << "Error opening file!\n";
            return;
        }
        // setprecisiion(2) is used to set the limit to 2 decimal places
        // fixed is used to display numbers in non scientific way (withou e and stuff like this)
        outFile << id << " " << username << " " << password << " " << phoneNumber << " " << email << " " << fixed << setprecision(2) << balance.getBalance() << endl;
        outFile.close();
        cout << "Account saved successfully!\n";
    }

    bool login() {
        string loginID, loginPass;
        cout << "Enter your ID: ";
        cin >> loginID;
        cout << "Enter your password: ";
        cin >> loginPass;

        // ifstream opens the file in read mode
        // FILE_PATH = where all students are saved
        ifstream inFile(FILE_PATH);
        if (!inFile) {
            cout << "No user data found.\n";
            return false;
        }

        string storedID, storedUsername, storedPass, storedPhone, storedEmail;
        double storedBalance;
        while (inFile >> storedID >> storedUsername >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if (loginID == storedID && loginPass == storedPass) {
                cout << "Login successful! Welcome, " << storedUsername << "!\n";
                id = storedID;
                username = storedUsername;
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

        // this will open the og file with all the students data
        ifstream inFile(FILE_PATH);
        // this will store all accounts except the one to delete.
        ofstream outFile("temp.txt");
        
        //  just a check to see if the file exists
        if (!inFile) {
            cout << "No user data found.\n";
            return false;
        }

        string storedID, storedUsername, storedPass, storedPhone, storedEmail;
        double storedBalance;
        bool found = false;

        while (inFile >> storedID >> storedUsername >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if ((delID == storedID || delID == storedEmail) && delPass == storedPass) {
                found = true;
                cout << "Account deleted successfully!\n";
            } 
            // if not the same account, copy this peice of data into the temp.txt file
            else {
                outFile << storedID << " " << storedUsername << " " << storedPass << " " << storedPhone << " " << storedEmail << " " << fixed << setprecision(2) << storedBalance << endl;
            }
        }

        inFile.close();
        outFile.close();

        if (!found) {
            cout << "Invalid ID/Email or password. Account not deleted.\n";
            // here we can remove the temp.txt file nonetheless as this contains only the copied data
            remove("temp.txt");
            return false;
        }

         //  in this part we will simply rename the temp.txt file (where all the data is copied except for the account that has to be deleted)
        // to students.txt. this way we have a new file with the name students.txt that does not contain the deleted accound
        remove(FILE_PATH.c_str());
        rename("temp.txt", FILE_PATH.c_str());
        return true;
    }

    bool isValidPhoneNumber(string phone) {
        if (phone.length() != 11) 
            return false;
        for (char c : phone) {
            if (!isdigit(c)) 
                return false;
        }
        return true;
    }
    // this is a check if the entered email is correct. (can find this on google easily)
    // the breakdown for this strinng statement:
    // | `^`                 | start of string                                 |
    // | `[a-zA-Z0-9._%+-]+` | username part (letters, numbers, ., _, %, +, -) |
    // | `@`                 | must contain '@'                                |
    // | `[a-zA-Z0-9.-]+`    | domain name (gmail, yahoo, hu, etc.)            |
    // | `\.`                | must contain a dot '.'                          |
    // | `[a-zA-Z]{2,}`      | domain extension: com, pk, edu, org, etc.       |
    // | `$`                 | end of string                                   |
    bool isValidEmail(string email) {
        regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return regex_match(email, emailRegex);
    }
// this updates the balacne of the student whos is currently logged in
void updateBalanceInFile() {
    ifstream inFile(FILE_PATH);
    ofstream outFile("temp.txt");
    
    if (!inFile) {
        cout << "Error opening file for reading!\n";
        return;
    }
    
    if (!outFile) {
        cout << "Error opening temp file for writing!\n";
        inFile.close();
        return;
    }
    
    string storedID, storedUsername, storedPass, storedPhone, storedEmail;
    double storedBalance;
    
    while (inFile >> storedID >> storedUsername >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
        // heree we will Update balance if this is the current logged-in student
        if (storedID == id) {
            storedBalance = balance.getBalance();
        }
        // Write to temp file
        outFile << storedID << " " << storedUsername << " " << storedPass << " " 
                << storedPhone << " " << storedEmail << " " 
                << fixed << setprecision(2) << storedBalance << endl;
    }
    
    inFile.close();
    outFile.close();

    // Replace original file with updated temp file
    // here we are renaming the temp.txt file as student.txt to store the previous data + the updated data taht has updated balance of the student

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
            // i added cin.ignore over here. its important as without this the code will be stuck in an infinite loop if a user enters an invalid payment choice
            // this is bcz even when the cin.clear() removes the value the incorrect value is still in buffer (cauisng an infinite loop)
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

        // Extract items and quantities from cart
        vector<Item> orderItems;
        vector<int> orderQuantities;
        for (auto &item : cart.items) {
            orderItems.push_back(Item(item.name, item.price));
            orderQuantities.push_back(item.quantity);
        }

        orderHistory.addOrder(orderItems, orderQuantities, total, paymentMethod, phoneNumber, username);
        cout << "Your order will be ready in about 1 minute!\n";
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
    menu.loadStockFromFile();
    Student student;
    int choice;

    std::cout << "GrabNob Bakery System\n";
    std::cout << "Your Campus Food Ordering App\n\n";
    
    std::cout << "1. Register New Account\n";
    std::cout << "2. Login\n";
    std::cout << "3. Delete Account\n";
    choice = getValidInput(1, 3);

    if (choice == 1) {
        std::cout << "\nRegistration:\n";
        student.inputDetails();
        student.saveToFile();
        std::cout << "Please login to continue.\n";
        return 0;
    } else if (choice == 3) {
        Student::deleteAccount();
        return 0;
    } else {
        std::cout << "\nLogin:\n";
        if (!student.login()) {
            return 0;
        }
  
        ingredientInventory.loadFromFile(); 
    }
    // we are using a do while loop to make sure interface is displayed till the user enters 0
    do {
        std::cout << "\nGrabNob Bakery\n";
        student.balance.displayBalance();
        std::cout << "\n1. Browse Menu\n";
        std::cout << "2. View Shopping Cart\n";
        std::cout << "3. Remove Item from Cart\n";
        std::cout << "4. Checkout\n";
        std::cout << "5. View Ongoing Orders\n";
        std::cout << "6. View Order History\n";
        std::cout << "7. Recharge Balance\n";
        std::cout << "0. Logout\n";
        choice = getValidInput(0, 7);

        switch (choice) {
            case 1: {
                menu.displayCategories();
                int cat = getValidInput(0, (int)menu.categories.size());
                if (cat > 0) {
                    // we have done "cat - 1" as we are accessing a vector categoies from here (basically input-ing index)
                    menu.displayCategoryMenu(cat - 1);
                    // item num is basically sabdwiches, burgers etc.
                    // get valid input will first check validity of the input and then insert in itemNum
                    int itemNum = getValidInput(0, (int)menu.categories[cat - 1].items.size());
                    if (itemNum > 0) {
                        Item *selected = menu.getItemRef(cat - 1, itemNum - 1);
                        if (!selected) 
                            break;

                        if (selected->name == "Make Your Own Sandwich") {
                            CustomSandwich cs;
                            Item customSample = cs.buildSandwich();
                            std::cout << "Enter quantity of this custom sandwich to add: ";
                            int qty;
                            while (!(cin >> qty) || qty <= 0) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                std::cout << "Invalid quantity. Enter positive integer: ";
                            }

                            if (!ingredientInventory.hasEnoughAll(cs.chosenIngredientKeys, qty)) {
                                std::cout << "Not enough ingredient stock for " << qty << " sandwich(es).\n";
                                // this loop tells the number of ingredients available 
                                for (auto &k : cs.chosenIngredientKeys) {
                                    std::cout << "   " << k << ": " << ingredientInventory.stock[k] << " available\n";
                                }
                            } 
                            else {
                                // if the requiered quantity is there, then we'll remove that many from the stock
                                // then we will create a vector with the key (item name) and qty of that item
                                // fianlly we will add that in our cart
                                ingredientInventory.consumeAll(cs.chosenIngredientKeys, qty);
                                vector<pair<string,int>> usage;
                                for (auto &k : cs.chosenIngredientKeys) 
                                    usage.push_back({k, qty});
                                student.cart.addItem(customSample.name, customSample.price, qty, true, usage);
                            }
                        } 
                        else {
                            std::cout << "Enter quantity to add: ";
                            int qty;
                            while (!(cin >> qty) || qty <= 0) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                std::cout << "Invalid quantity. Enter positive integer: ";
                            }
                            if (qty > selected->quantity) {
                                std::cout << "Not enough stock. Available: " << selected->quantity << "\n";
                            } 
                            else {
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
                    int removeIndex = getValidInput(0, (int)student.cart.items.size());
                    if (removeIndex > 0) {
                        CartItem &itemToRemove = student.cart.items[removeIndex - 1];
                        std::cout << "You have " << itemToRemove.quantity << " of " << itemToRemove.name << " in your cart.\n";
                        std::cout << "Enter quantity to remove (1-" << itemToRemove.quantity << "): ";
                        int qtyToRemove;
                        while (!(cin >> qtyToRemove) || qtyToRemove < 1 || qtyToRemove > itemToRemove.quantity) {
                            cin.clear();
                            cin.ignore(10000, '\n');
                            std::cout << "Invalid quantity. Enter a number between 1 and " << itemToRemove.quantity << ": ";
                        }
                        // restore stock if the item has beeen removed (this is for normal items + the ingredients for the custom sandwich)
                        if (itemToRemove.isCustom) {
                            vector<pair<string,int>> restoreUsage;
                            for (auto &p : itemToRemove.ingredientUsage) {
                                restoreUsage.push_back({p.first, qtyToRemove});
                            }
                            ingredientInventory.restoreAll(restoreUsage);
                        } 
                        else {
                            menu.increaseStockByName(itemToRemove.name, qtyToRemove);
                        }

                        itemToRemove.quantity -= qtyToRemove;
                        std::cout << "Removed " << qtyToRemove << " of " << itemToRemove.name << " from cart.\n";
                        
                        // if quantity reaches 0 then we will erase the item completely. erase is just a fucntion of vectors
                        if (itemToRemove.quantity == 0) {
                            student.cart.items.erase(student.cart.items.begin() + (removeIndex - 1));
                        }
                        menu.saveStockToFile();  
                    }
                }
                break;
            }

            case 4:
                student.checkout();
                menu.saveStockToFile();  
                break;
            case 5:
                student.orderHistory.displayOngoingOrders();
                break;
            case 6: {
                std::cout << "\n1. View All Orders\n";
                std::cout << "2. View Completed Orders\n";
                std::cout << "3. Track Specific Order\n";
                int histChoice = getValidInput(1, 3);
                if (histChoice == 1) {
                    student.orderHistory.displayAllOrders();
                } 
                else if (histChoice == 2) {
                    student.orderHistory.displayCompletedOrders();
                } 
                else {
                    string ordID;
                    std::cout << "Enter Order ID: ";
                    cin >> ordID;
                    student.orderHistory.trackOrder(ordID);
                }
                break;
            }
            case 7: {
                double rechargeAmount;
                std::cout << "Enter amount to recharge (Rs.): ";
                cin >> rechargeAmount;
                if (rechargeAmount > 0) {
                    student.balance.rechargeBalance(rechargeAmount);
                    student.updateBalanceInFile();
                } 
                else {
                    std::cout << "Invalid amount!\n";
                }
                break;
            }
            case 0:
                std::cout << "Logging out. Thank you for using GrabNob!\n";
                break;
        }

    } while (choice != 0);

    return 0;
}