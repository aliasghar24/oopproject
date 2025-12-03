// ----------------------------------------------------CODE 3----------------------------------
#include <SFML/Graphics.hpp>
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
#include <sstream>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

const string FILE_PATH = "Students.txt";
const string ORDERS_FILE = "Orders.txt";
const string MENU_STOCK_FILE = "MenuStock.txt";
const string INGREDIENT_STOCK_FILE = "IngredientStock.txt";

// ===================== INGREDIENT INVENTORY SYSTEM =====================
class IngredientInventory {
public:
    unordered_map<string, int> stock;

    IngredientInventory() {
        loadFromFile();
    }

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

    string getStockDisplayText() {
        stringstream ss;
        ss << "--- Ingredient Availability ---\n";
        ss << "Breads: White(" << stock["Bread_White"] << "), Brown(" << stock["Bread_Brown"]
           << "), Multigrain(" << stock["Bread_Multigrain"] << ")\n";
        ss << "Proteins: Chicken(" << stock["Protein_Chicken"] << "), Beef(" << stock["Protein_Beef"]
           << "), Egg(" << stock["Protein_Egg"] << ")\n";
        ss << "Veggies: Lettuce(" << stock["Veg_Lettuce"] << "), Cucumber(" << stock["Veg_Cucumber"]
           << "), Olives(" << stock["Veg_Olives"] << "), Onions(" << stock["Veg_Onions"] << ")\n";
        ss << "Sauces: Mayo(" << stock["Sauce_Mayo"] << "), Garlic(" << stock["Sauce_Garlic"]
           << "), Chipotle(" << stock["Sauce_Chipotle"] << ")\n";
        ss << "Extras: Cheese(" << stock["Extra_Cheese"] << ")\n";
        return ss.str();
    }

    void saveToFile() {
        ofstream outFile(INGREDIENT_STOCK_FILE);
        if (!outFile) return;
        
        for (auto &pair : stock) {
            outFile << pair.first << "||" << pair.second << "\n";
        }
        outFile.close();
    }

    void loadFromFile() {
        ifstream inFile(INGREDIENT_STOCK_FILE);
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

// ===================== CUSTOM SANDWICH BUILDER =====================
class CustomSandwich {
public:
    string breadKey, proteinKey, vegKey, sauceKey, extraKey;
    string bread, protein, veggies, sauce, extras;
    double price;
    vector<string> chosenIngredientKeys;
    vector<pair<string, int>> ingredientUsage;

    CustomSandwich() : price(0) {}

    bool buildSandwichGUI(int breadChoice, int proteinChoice, 
                         int vegChoice, int sauceChoice, int extraChoice) {
        chosenIngredientKeys.clear();
        ingredientUsage.clear();
        price = 0;

        // Bread
        if (breadChoice == 1) {
            bread = "White"; breadKey = "Bread_White"; price += 50;
        } else if (breadChoice == 2) {
            bread = "Brown"; breadKey = "Bread_Brown"; price += 60;
        } else if (breadChoice == 3) {
            bread = "Multigrain"; breadKey = "Bread_Multigrain"; price += 80;
        }
        chosenIngredientKeys.push_back(breadKey);

        // Protein
        if (proteinChoice == 1) { 
            protein = "Chicken"; proteinKey = "Protein_Chicken"; price += 150; 
            chosenIngredientKeys.push_back(proteinKey);
        }
        else if (proteinChoice == 2) { 
            protein = "Beef"; proteinKey = "Protein_Beef"; price += 200; 
            chosenIngredientKeys.push_back(proteinKey);
        }
        else if (proteinChoice == 3) { 
            protein = "Egg"; proteinKey = "Protein_Egg"; price += 70; 
            chosenIngredientKeys.push_back(proteinKey);
        }
        else { 
            protein = "None"; proteinKey = ""; 
        }

        // Veggies
        if (vegChoice == 1) { 
            veggies = "Lettuce"; vegKey = "Veg_Lettuce"; price += 40; 
            chosenIngredientKeys.push_back(vegKey);
        }
        else if (vegChoice == 2) { 
            veggies = "Cucumber"; vegKey = "Veg_Cucumber"; price += 50; 
            chosenIngredientKeys.push_back(vegKey);
        }
        else if (vegChoice == 3) { 
            veggies = "Olives"; vegKey = "Veg_Olives"; price += 30; 
            chosenIngredientKeys.push_back(vegKey);
        }
        else if (vegChoice == 4) { 
            veggies = "Onions"; vegKey = "Veg_Onions"; price += 20; 
            chosenIngredientKeys.push_back(vegKey);
        }
        else { 
            veggies = "None"; vegKey = ""; 
        }

        // Sauce
        if (sauceChoice == 1) { 
            sauce = "Mayo"; sauceKey = "Sauce_Mayo"; price += 20; 
            chosenIngredientKeys.push_back(sauceKey);
        }
        else if (sauceChoice == 2) { 
            sauce = "Garlic"; sauceKey = "Sauce_Garlic"; price += 20; 
            chosenIngredientKeys.push_back(sauceKey);
        }
        else if (sauceChoice == 3) { 
            sauce = "Chipotle"; sauceKey = "Sauce_Chipotle"; price += 20; 
            chosenIngredientKeys.push_back(sauceKey);
        }
        else { 
            sauce = "None"; sauceKey = ""; 
        }

        // Extras
        if (extraChoice == 1) { 
            extras = "Cheese"; extraKey = "Extra_Cheese"; price += 50; 
            chosenIngredientKeys.push_back(extraKey);
        }
        else { 
            extras = "None"; extraKey = ""; 
        }

        return true;
    }

    string getName() const {
        return "Custom Sandwich (" + bread + ", " + protein + ", " + veggies + ", " + sauce + ", " + extras + ")";
    }

    double getPrice() const {
        return price;
    }

    vector<pair<string, int>> getIngredientUsage(int quantity) const {
        vector<pair<string, int>> usage;
        for (auto &key : chosenIngredientKeys) {
            usage.push_back({key, quantity});
        }
        return usage;
    }

    vector<string> getChosenIngredients() const {
        return chosenIngredientKeys;
    }

    string getDescription() const {
        stringstream ss;
        ss << "Custom Sandwich\n";
        ss << "Bread: " << bread << "\n";
        ss << "Protein: " << protein << "\n";
        ss << "Veggies: " << veggies << "\n";
        ss << "Sauce: " << sauce << "\n";
        ss << "Extras: " << extras << "\n";
        ss << "Price: Rs." << price;
        return ss.str();
    }

    string getSummary() const {
        stringstream ss;
        ss << "Bread: " << bread << " | Protein: " << protein 
           << " | Veggies: " << veggies << " | Sauce: " << sauce 
           << " | Extras: " << extras << " | Price: Rs." << price;
        return ss.str();
    }
};

// ===================== GUI HELPER CLASSES =====================
class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;
    sf::Color normalColor;
    sf::Color hoverColor;
    sf::Color clickColor;
    bool isHovered;
    bool isClicked;

    Button(float x, float y, float width, float height, const string& label, sf::Font& font) {
        shape.setPosition(x, y);
        shape.setSize(sf::Vector2f(width, height));
        normalColor = sf::Color(70, 130, 180);
        hoverColor = sf::Color(100, 149, 237);
        clickColor = sf::Color(65, 105, 225);
        shape.setFillColor(normalColor);

        text.setString(label);
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setPosition(
            x + (width - textBounds.width) / 2.f,
            y + (height - textBounds.height) / 2.f - 5
        );
        
        isHovered = false;
        isClicked = false;
    }

    void update(sf::Vector2i mousePos) {
        isHovered = shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
        
        if (isClicked) {
            shape.setFillColor(clickColor);
        } else if (isHovered) {
            shape.setFillColor(hoverColor);
        } else {
            shape.setFillColor(normalColor);
        }
    }

    bool isPressed(sf::Event& event, sf::Vector2i mousePos) {
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left && isHovered) {
                isClicked = true;
                return false;
            }
        }
        
        if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left && isClicked) {
                isClicked = false;
                return isHovered;
            }
        }
        
        return false;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

    void setLabel(const string& newLabel) {
        text.setString(newLabel);
        sf::FloatRect textBounds = text.getLocalBounds();
        sf::Vector2f pos = shape.getPosition();
        sf::Vector2f size = shape.getSize();
        text.setPosition(
            pos.x + (size.x - textBounds.width) / 2.f,
            pos.y + (size.y - textBounds.height) / 2.f - 5
        );
    }

    void setPosition(float x, float y) {
        sf::Vector2f oldPos = shape.getPosition();
        sf::Vector2f delta(x - oldPos.x, y - oldPos.y);
        shape.setPosition(x, y);
        text.setPosition(text.getPosition().x + delta.x, text.getPosition().y + delta.y);
    }
};

class InputBox {
public:
    sf::RectangleShape box;
    sf::Text displayText;
    sf::Text label;
    string content;
    bool isActive;
    bool isPassword;
    sf::Clock blinkClock;
    bool showCursor;

    InputBox(float x, float y, float width, float height, const string& labelText, sf::Font& font, bool password = false) {
        box.setPosition(x, y);
        box.setSize(sf::Vector2f(width, height));
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(100, 100, 100));

        label.setString(labelText);
        label.setFont(font);
        label.setCharacterSize(18);
        label.setFillColor(sf::Color::White);
        label.setPosition(x, y - 30);

        displayText.setFont(font);
        displayText.setCharacterSize(20);
        displayText.setFillColor(sf::Color::Black);
        displayText.setPosition(x + 10, y + 10);

        content = "";
        isActive = false;
        isPassword = password;
        showCursor = true;
    }

    void setActive(bool active) {
        isActive = active;
        if (active) {
            box.setOutlineColor(sf::Color(70, 130, 180));
        } else {
            box.setOutlineColor(sf::Color(100, 100, 100));
        }
    }

    void handleInput(sf::Event& event) {
        if (!isActive) return;

        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8 && content.length() > 0) { // Backspace
                content.pop_back();
            } else if (event.text.unicode >= 32 && event.text.unicode < 128) {
                content += static_cast<char>(event.text.unicode);
            }
            updateDisplay();
        }
    }

    void updateDisplay() {
        if (isPassword && !content.empty()) {
            displayText.setString(string(content.length(), '*'));
        } else {
            displayText.setString(content);
        }
    }

    void update() {
        if (blinkClock.getElapsedTime().asSeconds() > 0.5f) {
            showCursor = !showCursor;
            blinkClock.restart();
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(label);
        window.draw(box);
        window.draw(displayText);
        
        if (isActive && showCursor) {
            sf::RectangleShape cursor(sf::Vector2f(2, 20));
            cursor.setFillColor(sf::Color::Black);
            sf::FloatRect textBounds = displayText.getGlobalBounds();
            cursor.setPosition(textBounds.left + textBounds.width + 2, box.getPosition().y + 10);
            window.draw(cursor);
        }
    }

    void clear() {
        content = "";
        updateDisplay();
    }

    int getIntValue() {
        try {
            return stoi(content);
        } catch (...) {
            return 0;
        }
    }

    float getFloatValue() {
        try {
            return stof(content);
        } catch (...) {
            return 0.0f;
        }
    }
};

// ===================== ORIGINAL CLASSES (MODIFIED) =====================
class Item {
public:
    string name;
    double price;
    int quantity;
    bool isCustom;
    vector<pair<string, int>> ingredientUsage;
    
    Item(string n = "", double p = 0, int q = 5, bool custom = false) : 
        name(n), price(p), quantity(q), isCustom(custom) {}
};

class CartItem {
public:
    string name;
    double price;
    int quantity;
    bool isCustom;
    vector<pair<string, int>> ingredientUsage;

    CartItem(string n = "", double p = 0, int q = 0, bool custom = false) : 
        name(n), price(p), quantity(q), isCustom(custom) {}
};

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

    Order(string id, string user, string date, vector<Item> cartItems, vector<int> qtys, 
          double total, string method, string phone) 
        : orderID(id), username(user), orderDate(date), items(cartItems), quantities(qtys), 
          totalAmount(total), readyTimeSeconds(60), status("Preparing"), 
          paymentMethod(method), phoneNumber(phone) {
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

    string getTimeRemainingString() {
        int remaining = getTimeRemaining();
        if (remaining <= 0) return "00:00";
        
        int minutes = remaining / 60;
        int seconds = remaining % 60;
        stringstream ss;
        ss << setfill('0') << setw(2) << minutes << ":" << setw(2) << seconds;
        return ss.str();
    }

    string getDisplayString() {
        updateStatus();
        stringstream ss;
        ss << "Order ID: " << orderID << " | Customer: " << username 
           << " | Total: Rs." << totalAmount << " | Status: " << status;
        
        if (status == "Preparing") {
            ss << " | Time Remaining: " << getTimeRemainingString();
        }
        return ss.str();
    }

    string getDetailedString() {
        updateStatus();
        stringstream ss;
        ss << "Order ID: " << orderID << "\n";
        ss << "Customer: " << username << "\n";
        ss << "Date: " << orderDate << "\n";
        ss << "Total: Rs." << totalAmount << "\n";
        ss << "Payment: " << paymentMethod << "\n";
        ss << "Phone: " << phoneNumber << "\n";
        ss << "Status: " << status << "\n";
        
        if (status == "Preparing") {
            ss << "Time Remaining: " << getTimeRemainingString() << "\n";
        }
        
        ss << "Items:\n";
        for (size_t i = 0; i < items.size(); i++) {
            ss << "  " << i + 1 << ". " << items[i].name << " x" << quantities[i] 
               << " - Rs." << items[i].price << " each\n";
        }
        
        return ss.str();
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
        if (!inFile) return;

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

    void addOrder(vector<Item> items, vector<int> quantities, double total, 
                  string method, string phone, string username) {
        string ordID = generateOrderID();
        string date = getCurrentDate();
        Order newOrder(ordID, username, date, items, quantities, total, method, phone);
        orders.push_back(newOrder);
        saveOrderToFile(newOrder);
    }

    void saveOrderToFile(Order &order) {
        ofstream outFile(ORDERS_FILE, ios::app);
        if (!outFile) return;
        
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
        if (!inFile) return;

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
                continue;
            }
        }
        inFile.close();
    }

    vector<Order>& getAllOrders() {
        return orders;
    }

    vector<Order> getOngoingOrders() {
        vector<Order> ongoing;
        for (auto &order : orders) {
            order.updateStatus();
            if (order.getTimeRemaining() > 0) {
                ongoing.push_back(order);
            }
        }
        return ongoing;
    }

    vector<Order> getCompletedOrders() {
        vector<Order> completed;
        for (auto &order : orders) {
            order.updateStatus();
            if (order.getTimeRemaining() <= 0) {
                completed.push_back(order);
            }
        }
        return completed;
    }

    // ========== NEW METHODS ADDED ==========
    vector<Order> getOrdersForUser(const string& username) {
        vector<Order> userOrders;
        for (auto &order : orders) {
            if (order.username == username) {
                userOrders.push_back(order);
            }
        }
        return userOrders;
    }

    vector<Order> getOngoingOrdersForUser(const string& username) {
        vector<Order> ongoing;
        for (auto &order : orders) {
            order.updateStatus();
            if (order.getTimeRemaining() > 0 && order.username == username) {
                ongoing.push_back(order);
            }
        }
        return ongoing;
    }

    vector<Order> getCompletedOrdersForUser(const string& username) {
        vector<Order> completed;
        for (auto &order : orders) {
            order.updateStatus();
            if (order.getTimeRemaining() <= 0 && order.username == username) {
                completed.push_back(order);
            }
        }
        return completed;
    }

    Order* findOrderByID(const string& ordID) {
        for (auto& order : orders) {
            if (order.orderID == ordID) {
                return &order;
            }
        }
        return nullptr;
    }
};
class Menu {
public:
    vector<vector<Item>> categories;
    vector<string> categoryNames;

    Menu() {
        categoryNames = {"Savoury", "Sweets", "Beverages"};
        
        vector<Item> savoury = {
            Item("Sandwich", 350, 5),
            Item("Burger", 400, 5),
            Item("Pizza Slice", 450, 5),
            Item("Make Your Own Sandwich", 100, 1, true)
        };
        
        vector<Item> sweets = {
            Item("Cookie", 250, 5),
            Item("Brownie", 300, 5),
            Item("Donut", 200, 5)
        };
        
        vector<Item> beverages = {
            Item("Coffee", 400, 5),
            Item("Pepsi", 60, 5),
            Item("Tea", 150, 5),
            Item("Juice", 200, 5)
        };
        
        categories.push_back(savoury);
        categories.push_back(sweets);
        categories.push_back(beverages);
        
        loadStockFromFile();
    }

    void increaseStockByName(const string &itemName, int qty) {
        for (auto &cat : categories) {
            for (auto &it : cat) {
                if (it.name == itemName) {
                    it.quantity += qty;
                    return;
                }
            }
        }
    }

    Item* getItemByName(const string& itemName) {
        for (auto &cat : categories) {
            for (auto &it : cat) {
                if (it.name == itemName) {
                    return &it;
                }
            }
        }
        return nullptr;
    }

    void saveStockToFile() {
        ofstream outFile(MENU_STOCK_FILE);
        if (!outFile) return;
        
        for (auto &cat : categories) {
            for (auto &item : cat) {
                outFile << item.name << "||" << item.quantity << "\n";
            }
        }
        outFile.close();
    }

    void loadStockFromFile() {
        ifstream inFile(MENU_STOCK_FILE);
        if (!inFile) {
            saveStockToFile();
            return;
        }
        
        string line;
        while (getline(inFile, line)) {
            size_t pos = line.find("||");
            if (pos != string::npos) {
                string itemName = line.substr(0, pos);
                int quantity = stoi(line.substr(pos + 2));
                
                for (auto &cat : categories) {
                    for (auto &item : cat) {
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

class Student {
public:
    string id;
    string username;
    string password;
    string phoneNumber;
    string email;
    double balance;
    vector<CartItem> cart;
    OrderHistory orderHistory;

    Student() : balance(1000) {}

    bool login(const string& loginID, const string& loginPass) {
        ifstream inFile(FILE_PATH);
        if (!inFile) return false;

        string storedID, storedUsername, storedPass, storedPhone, storedEmail;
        double storedBalance;
        
        while (inFile >> storedID >> storedUsername >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if (loginID == storedID && loginPass == storedPass) {
                id = storedID;
                username = storedUsername;
                password = storedPass;
                phoneNumber = storedPhone;
                email = storedEmail;
                balance = storedBalance;
                inFile.close();
                return true;
            }
        }
        inFile.close();
        return false;
    }

    bool registerAccount(const string& newID, const string& newUsername, const string& newPass, 
                        const string& newPhone, const string& newEmail, double initialBalance) {
        ofstream outFile(FILE_PATH, ios::app);
        if (!outFile) return false;
        
        outFile << newID << " " << newUsername << " " << newPass << " " 
                << newPhone << " " << newEmail << " " << fixed << setprecision(2) 
                << initialBalance << endl;
        outFile.close();
        return true;
    }

    void addToCart(const string &name, double price, int qty, bool isCustom = false, 
                   const vector<pair<string, int>>& ingredientUsage = {}) {
        for (auto &ci : cart) {
            if (ci.name == name && ci.isCustom == isCustom) {
                ci.quantity += qty;
                return;
            }
        }
        CartItem item(name, price, qty, isCustom);
        if (isCustom) {
            item.ingredientUsage = ingredientUsage;
        }
        cart.push_back(item);
    }

    double getCartTotal() {
        double total = 0;
        for (auto &item : cart) {
            total += item.price * item.quantity;
        }
        return total;
    }

    void clearCart() {
        cart.clear();
    }

    void updateBalanceInFile() {
        ifstream inFile(FILE_PATH);
        ofstream outFile("temp.txt");
        
        if (!inFile || !outFile) return;
        
        string storedID, storedUsername, storedPass, storedPhone, storedEmail;
        double storedBalance;
        
        while (inFile >> storedID >> storedUsername >> storedPass >> storedPhone >> storedEmail >> storedBalance) {
            if (storedID == id) {
                storedBalance = balance;
            }
            outFile << storedID << " " << storedUsername << " " << storedPass << " " 
                    << storedPhone << " " << storedEmail << " " 
                    << fixed << setprecision(2) << storedBalance << endl;
        }
        
        inFile.close();
        outFile.close();
        
        remove(FILE_PATH.c_str());
        rename("temp.txt", FILE_PATH.c_str());
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

    bool isValidEmail(string email) {
        regex emailRegex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
        return regex_match(email, emailRegex);
    }
};

// ===================== GUI SCREENS =====================
enum Screen { LOGIN, REGISTER, MENU, BROWSE, CART, CHECKOUT, ORDERS, RECHARGE, CUSTOM_SANDWICH };

class GrabNobGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Screen currentScreen;
    Student currentStudent;
    Menu menu;
    IngredientInventory ingredientInventory;
    
    // Custom Sandwich Builder variables
    int customSandwichStep;
    int breadChoice, proteinChoice, vegChoice, sauceChoice, extraChoice;
    int customQuantity;
    CustomSandwich currentCustomSandwich;
    
    // Login Screen
    InputBox* loginIDBox;
    InputBox* loginPassBox;
    Button* loginButton;
    Button* registerNavButton;
    
    // Register Screen
    InputBox* regIDBox;
    InputBox* regUsernameBox;
    InputBox* regPassBox;
    InputBox* regPhoneBox;
    InputBox* regEmailBox;
    InputBox* regBalanceBox;
    Button* registerButton;
    Button* backToLoginButton;
    
    // Menu Screen
    vector<Button*> menuButtons;
    Button* logoutButton;
    
    // Browse Screen
    vector<Button*> categoryButtons;
    vector<Button*> itemButtons;
    Button* backToMenuButton;
    int selectedCategory;
    
    // Cart Screen
    vector<Button*> cartItemButtons;
    vector<Button*> removeButtons;
    Button* checkoutButton;
    Button* backToMenuFromCart;
    
    // Checkout Screen
    Button* payOnlineButton;
    Button* payCardButton;
    Button* payCashButton;
    Button* confirmOrderButton;
    Button* backToCartButton;
    int selectedPaymentMethod;
    
    // Orders Screen
    Button* viewAllOrdersButton;
    Button* viewOngoingOrdersButton;
    Button* viewCompletedOrdersButton;
    Button* trackOrderButton;
    Button* backToMenuFromOrders;
    InputBox* trackOrderIDBox;
    int ordersScrollOffset;
    bool showAllOrders;
    bool showOngoingOrders;
    bool showCompletedOrders;
    bool showTrackOrder;
    
    // Custom Sandwich Screen
    vector<Button*> customSandwichButtons;
    vector<Button*> customSandwichButtonsStep1; // Bread
    vector<Button*> customSandwichButtonsStep2; // Protein
    vector<Button*> customSandwichButtonsStep3; // Veggies
    vector<Button*> customSandwichButtonsStep4; // Sauce
    vector<Button*> customSandwichButtonsStep5; // Extras
    Button* nextStepButton;
    Button* prevStepButton;
    Button* finishCustomButton;
    Button* cancelCustomButton;
    InputBox* quantityInputBox;
    vector<string> customStepLabels;
    
    
    // Recharge Screen
    InputBox* rechargeAmountBox;
    Button* rechargeConfirmButton;
    Button* backToMenuFromRecharge;
    
    sf::Text messageText;
    sf::Clock messageClock;
    string messageContent;

public:
    GrabNobGUI() : window(sf::VideoMode(900, 700), "GrabNob Bakery System"), 
               currentScreen(LOGIN), selectedCategory(-1), selectedPaymentMethod(0), 
               ordersScrollOffset(0), showAllOrders(true), showOngoingOrders(false),
               showCompletedOrders(false), showTrackOrder(false),
               customSandwichStep(0), breadChoice(0), proteinChoice(0), 
               vegChoice(0), sauceChoice(0), extraChoice(0), customQuantity(1) {
    
        cout << "Starting GrabNob Bakery System..." << endl;
        window.setFramerateLimit(60);
        
        // Try to load font
        bool fontLoaded = false;
        
        if (font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            cout << "Font loaded successfully from Windows directory" << endl;
            fontLoaded = true;
        } else if (font.loadFromFile("arial.ttf")) {
            cout << "Font loaded from local directory" << endl;
            fontLoaded = true;
        } else if (font.loadFromFile("C:\\Windows\\Fonts\\calibri.ttf")) {
            cout << "Font loaded from calibri" << endl;
            fontLoaded = true;
        }
        
        if (!fontLoaded) {
            cerr << "ERROR: Could not load any font file!" << endl;
            cerr << "Please ensure arial.ttf exists in program directory or C:\\Windows\\Fonts\\" << endl;
            exit(1);
        }
        
        // Initialize custom step labels
        customStepLabels = {
            "Step 1: Choose Bread",
            "Step 2: Choose Protein",
            "Step 3: Choose Veggies", 
            "Step 4: Choose Sauce",
            "Step 5: Choose Extras",
            "Step 6: Enter Quantity"
        };
        
        messageText.setFont(font);
        
        initLoginScreen();
        initRegisterScreen();
        initMenuScreen();
        initBrowseScreen();
        initCartScreen();
        initCheckoutScreen();
        initOrdersScreen();
        initCustomSandwichScreen();
        initRechargeScreen();
        
        messageText.setCharacterSize(16);
        messageText.setFillColor(sf::Color::Red);
        messageText.setPosition(50, 650);
        
        cout << "GUI initialized successfully!" << endl;
    }

    ~GrabNobGUI() {
        // Save data on exit
        ingredientInventory.saveToFile();
        menu.saveStockToFile();
        
        // Clean up dynamic allocations
        delete loginIDBox;
        delete loginPassBox;
        delete loginButton;
        delete registerNavButton;
        
        delete regIDBox;
        delete regUsernameBox;
        delete regPassBox;
        delete regPhoneBox;
        delete regEmailBox;
        delete regBalanceBox;
        delete registerButton;
        delete backToLoginButton;
        
        for (auto btn : menuButtons) delete btn;
        delete logoutButton;
        
        for (auto btn : categoryButtons) delete btn;
        for (auto btn : itemButtons) delete btn;
        delete backToMenuButton;
        
        for (auto btn : cartItemButtons) delete btn;
        for (auto btn : removeButtons) delete btn;
        delete checkoutButton;
        delete backToMenuFromCart;
        
        delete payOnlineButton;
        delete payCardButton;
        delete payCashButton;
        delete confirmOrderButton;
        delete backToCartButton;
        
        delete viewAllOrdersButton;
        delete viewOngoingOrdersButton;
        delete viewCompletedOrdersButton;
        delete trackOrderButton;
        delete backToMenuFromOrders;
        delete trackOrderIDBox;
        
        for (auto btn : customSandwichButtons) delete btn;
        for (auto btn : customSandwichButtonsStep1) delete btn;
        for (auto btn : customSandwichButtonsStep2) delete btn;
        for (auto btn : customSandwichButtonsStep3) delete btn;
        for (auto btn : customSandwichButtonsStep4) delete btn;
        for (auto btn : customSandwichButtonsStep5) delete btn;
        delete nextStepButton;
        delete prevStepButton;
        delete finishCustomButton;
        delete cancelCustomButton;
        delete quantityInputBox;
        
        delete rechargeAmountBox;
        delete rechargeConfirmButton;
        delete backToMenuFromRecharge;
    }

    void initLoginScreen() {
        loginIDBox = new InputBox(300, 200, 300, 40, "Student ID:", font);
        loginPassBox = new InputBox(300, 280, 300, 40, "Password:", font, true);
        loginButton = new Button(350, 360, 200, 50, "Login", font);
        registerNavButton = new Button(350, 430, 200, 50, "Register", font);
    }

    void initRegisterScreen() {
        regIDBox = new InputBox(300, 100, 300, 35, "Student ID:", font);
        regUsernameBox = new InputBox(300, 160, 300, 35, "Username:", font);
        regPassBox = new InputBox(300, 220, 300, 35, "Password:", font, true);
        regPhoneBox = new InputBox(300, 280, 300, 35, "Phone (11 digits):", font);
        regEmailBox = new InputBox(300, 340, 300, 35, "Email:", font);
        regBalanceBox = new InputBox(300, 400, 300, 35, "Initial Balance:", font);
        registerButton = new Button(300, 460, 140, 45, "Register", font);
        backToLoginButton = new Button(460, 460, 140, 45, "Back", font);
    }

    void initMenuScreen() {
        menuButtons.push_back(new Button(300, 200, 300, 60, "Browse Menu", font));
        menuButtons.push_back(new Button(300, 280, 300, 60, "View Cart", font));
        menuButtons.push_back(new Button(300, 360, 300, 60, "My Orders", font));
        menuButtons.push_back(new Button(300, 440, 300, 60, "Recharge Balance", font));
        logoutButton = new Button(300, 520, 300, 60, "Logout", font);
    }

    void initBrowseScreen() {
        for (size_t i = 0; i < 3; i++) {
            categoryButtons.push_back(new Button(50, 100 + i * 80, 200, 60, 
                menu.categoryNames[i], font));
        }
        backToMenuButton = new Button(50, 550, 200, 50, "Back to Menu", font);
    }

    void initCartScreen() {
        checkoutButton = new Button(550, 600, 200, 50, "Checkout", font);
        backToMenuFromCart = new Button(100, 600, 200, 50, "Back to Menu", font);
    }

    void initCheckoutScreen() {
        payOnlineButton = new Button(300, 250, 300, 50, "Pay Online (Wallet)", font);
        payCardButton = new Button(300, 320, 300, 50, "Pay with Card", font);
        payCashButton = new Button(300, 390, 300, 50, "Pay with Cash", font);
        confirmOrderButton = new Button(300, 500, 300, 60, "Confirm Order", font);
        backToCartButton = new Button(300, 580, 300, 50, "Back to Cart", font);
    }

//  MAAZ CHANGED THIS PIECE OF CODE
    void initOrdersScreen() {
        // Button dimensions - slightly smaller to fit better
        float buttonWidth = 160;  // Reduced width
        float buttonHeight = 45;  // Slightly shorter
        float buttonSpacing = 8;  // Reduced spacing
        float yPosition = 620;    // A bit higher from bottom
        
        // Calculate starting x position
        float totalWidth = (buttonWidth * 5) + (buttonSpacing * 4);
        float startX = (900 - totalWidth) / 2;
        
        // Create buttons
        viewAllOrdersButton = new Button(startX, yPosition, buttonWidth, buttonHeight, "All", font);
        viewOngoingOrdersButton = new Button(startX + buttonWidth + buttonSpacing, yPosition, 
                                            buttonWidth, buttonHeight, "Ongoing", font);
        viewCompletedOrdersButton = new Button(startX + (buttonWidth * 2) + (buttonSpacing * 2), yPosition,
                                            buttonWidth, buttonHeight, "Completed", font);
        trackOrderButton = new Button(startX + (buttonWidth * 3) + (buttonSpacing * 3), yPosition,
                                    buttonWidth, buttonHeight, "Track", font);
        backToMenuFromOrders = new Button(startX + (buttonWidth * 4) + (buttonSpacing * 4), yPosition,
                                        buttonWidth, buttonHeight, "Back", font);
        
        trackOrderIDBox = new InputBox(300, 150, 300, 40, "Enter Order ID:", font);
    }

    void initCustomSandwichScreen() {
        // Step 1: Bread selection
        customSandwichButtonsStep1.push_back(new Button(300, 200, 300, 50, "White Bread (Rs. 50)", font));
        customSandwichButtonsStep1.push_back(new Button(300, 260, 300, 50, "Brown Bread (Rs. 60)", font));
        customSandwichButtonsStep1.push_back(new Button(300, 320, 300, 50, "Multigrain Bread (Rs. 80)", font));
        
        // Step 2: Protein selection
        customSandwichButtonsStep2.push_back(new Button(300, 200, 300, 50, "Chicken (Rs. 150)", font));
        customSandwichButtonsStep2.push_back(new Button(300, 260, 300, 50, "Beef (Rs. 200)", font));
        customSandwichButtonsStep2.push_back(new Button(300, 320, 300, 50, "Egg (Rs. 70)", font));
        customSandwichButtonsStep2.push_back(new Button(300, 380, 300, 50, "No Protein (Rs. 0)", font));
        
        // Step 3: Veggies selection
        customSandwichButtonsStep3.push_back(new Button(300, 200, 300, 50, "Lettuce (Rs. 40)", font));
        customSandwichButtonsStep3.push_back(new Button(300, 260, 300, 50, "Cucumber (Rs. 50)", font));
        customSandwichButtonsStep3.push_back(new Button(300, 320, 300, 50, "Olives (Rs. 30)", font));
        customSandwichButtonsStep3.push_back(new Button(300, 380, 300, 50, "Onions (Rs. 20)", font));
        customSandwichButtonsStep3.push_back(new Button(300, 440, 300, 50, "No Veggies (Rs. 0)", font));
        
        // Step 4: Sauce selection
        customSandwichButtonsStep4.push_back(new Button(300, 200, 300, 50, "Mayo (Rs. 20)", font));
        customSandwichButtonsStep4.push_back(new Button(300, 260, 300, 50, "Garlic (Rs. 20)", font));
        customSandwichButtonsStep4.push_back(new Button(300, 320, 300, 50, "Chipotle (Rs. 20)", font));
        customSandwichButtonsStep4.push_back(new Button(300, 380, 300, 50, "No Sauce (Rs. 0)", font));
        
        // Step 5: Extras selection
        customSandwichButtonsStep5.push_back(new Button(300, 200, 300, 50, "Cheese (Rs. 50)", font));
        customSandwichButtonsStep5.push_back(new Button(300, 260, 300, 50, "No Extras (Rs. 0)", font));
        
        nextStepButton = new Button(600, 550, 150, 50, "Next", font);
        prevStepButton = new Button(150, 550, 150, 50, "Back", font);
        finishCustomButton = new Button(300, 450, 300, 50, "Add to Cart", font);
        cancelCustomButton = new Button(300, 520, 300, 50, "Cancel", font);
        
        quantityInputBox = new InputBox(300, 350, 300, 40, "Quantity:", font);
    }

    void initRechargeScreen() {
        rechargeAmountBox = new InputBox(300, 250, 300, 40, "Recharge Amount (Rs.):", font);
        rechargeConfirmButton = new Button(300, 350, 300, 50, "Confirm Recharge", font);
        backToMenuFromRecharge = new Button(300, 420, 300, 50, "Back to Menu", font);
    }

    void showMessage(const string& msg, sf::Color color = sf::Color::Red) {
        messageContent = msg;
        messageText.setString(msg);
        messageText.setFillColor(color);
        messageClock.restart();
    }

    void handleLoginScreen(sf::Event& event, sf::Vector2i mousePos) {
        loginIDBox->handleInput(event);
        loginPassBox->handleInput(event);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                loginIDBox->setActive(loginIDBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
                loginPassBox->setActive(loginPassBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
            }
        }
        
        if (loginButton->isPressed(event, mousePos)) {
            if (currentStudent.login(loginIDBox->content, loginPassBox->content)) {
                showMessage("Login successful! Welcome " + currentStudent.username, sf::Color::Green);
                currentScreen = MENU;
                loginIDBox->clear();
                loginPassBox->clear();
            } else {
                showMessage("Invalid ID or password!");
            }
        }
        
        if (registerNavButton->isPressed(event, mousePos)) {
            currentScreen = REGISTER;
            showMessage("");
        }
    }

    void handleRegisterScreen(sf::Event& event, sf::Vector2i mousePos) {
        regIDBox->handleInput(event);
        regUsernameBox->handleInput(event);
        regPassBox->handleInput(event);
        regPhoneBox->handleInput(event);
        regEmailBox->handleInput(event);
        regBalanceBox->handleInput(event);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                regIDBox->setActive(regIDBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
                regUsernameBox->setActive(regUsernameBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
                regPassBox->setActive(regPassBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
                regPhoneBox->setActive(regPhoneBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
                regEmailBox->setActive(regEmailBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
                regBalanceBox->setActive(regBalanceBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
            }
        }
        
        if (registerButton->isPressed(event, mousePos)) {
            // Validate inputs
            if (regIDBox->content.empty() || regUsernameBox->content.empty() || 
                regPassBox->content.empty() || regPhoneBox->content.empty() || 
                regEmailBox->content.empty() || regBalanceBox->content.empty()) {
                showMessage("Please fill all fields!");
                return;
            }
            
            if (!currentStudent.isValidPhoneNumber(regPhoneBox->content)) {
                showMessage("Invalid phone number! Must be 11 digits.");
                return;
            }
            
            if (!currentStudent.isValidEmail(regEmailBox->content)) {
                showMessage("Invalid email address!");
                return;
            }
            
            try {
                double balance = stod(regBalanceBox->content);
                if (balance < 0) {
                    showMessage("Balance cannot be negative!");
                    return;
                }
                
                if (currentStudent.registerAccount(regIDBox->content, regUsernameBox->content,
                    regPassBox->content, regPhoneBox->content, regEmailBox->content, balance)) {
                    showMessage("Registration successful! Please login.", sf::Color::Green);
                    currentScreen = LOGIN;
                    regIDBox->clear();
                    regUsernameBox->clear();
                    regPassBox->clear();
                    regPhoneBox->clear();
                    regEmailBox->clear();
                    regBalanceBox->clear();
                } else {
                    showMessage("Registration failed!");
                }
            } catch (...) {
                showMessage("Invalid balance amount!");
            }
        }
        
        if (backToLoginButton->isPressed(event, mousePos)) {
            currentScreen = LOGIN;
            showMessage("");
        }
    }

    void handleMenuScreen(sf::Event& event, sf::Vector2i mousePos) {
        if (menuButtons[0]->isPressed(event, mousePos)) {
            currentScreen = BROWSE;
            selectedCategory = -1;
            itemButtons.clear();
        }
        
        if (menuButtons[1]->isPressed(event, mousePos)) {
            currentScreen = CART;
            refreshCartButtons();
        }
        
        if (menuButtons[2]->isPressed(event, mousePos)) {
            currentScreen = ORDERS;
            ordersScrollOffset = 0;
            showAllOrders = true;
            showOngoingOrders = false;
            showCompletedOrders = false;
            showTrackOrder = false;
        }
        
        if (menuButtons[3]->isPressed(event, mousePos)) {
            currentScreen = RECHARGE;
            rechargeAmountBox->clear();
        }
        
        if (logoutButton->isPressed(event, mousePos)) {
            currentScreen = LOGIN;
            currentStudent = Student();
            showMessage("Logged out successfully.", sf::Color::Green);
        }
    }

    void handleBrowseScreen(sf::Event& event, sf::Vector2i mousePos) {
        for (size_t i = 0; i < categoryButtons.size(); i++) {
            if (categoryButtons[i]->isPressed(event, mousePos)) {
                selectedCategory = i;
                
                // Create item buttons
                itemButtons.clear();
                for (size_t j = 0; j < menu.categories[i].size(); j++) {
                    stringstream ss;
                    ss << menu.categories[i][j].name << " - Rs." << menu.categories[i][j].price 
                       << " (Stock: " << menu.categories[i][j].quantity << ")";
                    itemButtons.push_back(new Button(300, 100 + j * 70, 550, 55, ss.str(), font));
                }
            }
        }
        
        for (size_t i = 0; i < itemButtons.size(); i++) {
            if (itemButtons[i]->isPressed(event, mousePos)) {
                if (menu.categories[selectedCategory][i].name == "Make Your Own Sandwich") {
                    // Start custom sandwich builder
                    currentScreen = CUSTOM_SANDWICH;
                    customSandwichStep = 0;
                    breadChoice = proteinChoice = vegChoice = sauceChoice = extraChoice = 0;
                    customQuantity = 1;
                    quantityInputBox->clear();
                    currentCustomSandwich = CustomSandwich();
                    showMessage("Start building your custom sandwich!", sf::Color::Blue);
                } else {
                    // Regular item
                    if (menu.categories[selectedCategory][i].quantity > 0) {
                        menu.categories[selectedCategory][i].quantity--;
                        currentStudent.addToCart(
                            menu.categories[selectedCategory][i].name,
                            menu.categories[selectedCategory][i].price,
                            1
                        );
                        showMessage("Added " + menu.categories[selectedCategory][i].name + " to cart!", sf::Color::Green);
                        
                        // Update button text
                        stringstream ss;
                        ss << menu.categories[selectedCategory][i].name << " - Rs." 
                           << menu.categories[selectedCategory][i].price 
                           << " (Stock: " << menu.categories[selectedCategory][i].quantity << ")";
                        itemButtons[i]->setLabel(ss.str());
                    } else {
                        showMessage("Item out of stock!");
                    }
                }
            }
        }
        
        if (backToMenuButton->isPressed(event, mousePos)) {
            currentScreen = MENU;
            selectedCategory = -1;
            itemButtons.clear();
        }
    }

    void refreshCartButtons() {
        for (auto btn : cartItemButtons) delete btn;
        for (auto btn : removeButtons) delete btn;
        cartItemButtons.clear();
        removeButtons.clear();

        for (size_t i = 0; i < currentStudent.cart.size(); i++) {
            stringstream ss;
            ss << currentStudent.cart[i].name << " x" << currentStudent.cart[i].quantity 
               << " - Rs." << currentStudent.cart[i].price << " each";
            
            cartItemButtons.push_back(new Button(100, 100 + i * 70, 500, 55, ss.str(), font));
            removeButtons.push_back(new Button(620, 100 + i * 70, 150, 55, "Remove", font));
        }
    }

    void handleCartScreen(sf::Event& event, sf::Vector2i mousePos) {
        for (size_t i = 0; i < removeButtons.size(); i++) {
            if (removeButtons[i]->isPressed(event, mousePos)) {
                // Restore stock for regular items
                if (!currentStudent.cart[i].isCustom) {
                    menu.increaseStockByName(currentStudent.cart[i].name, currentStudent.cart[i].quantity);
                } else {
                    // Restore ingredients for custom sandwiches
                    ingredientInventory.restoreAll(currentStudent.cart[i].ingredientUsage);
                }
                
                // Remove from cart
                currentStudent.cart.erase(currentStudent.cart.begin() + i);
                showMessage("Item removed from cart!", sf::Color::Green);
                refreshCartButtons();
                break;
            }
        }

        if (checkoutButton->isPressed(event, mousePos)) {
            if (currentStudent.cart.empty()) {
                showMessage("Cart is empty!");
            } else {
                currentScreen = CHECKOUT;
                selectedPaymentMethod = 0;
            }
        }

        if (backToMenuFromCart->isPressed(event, mousePos)) {
            currentScreen = MENU;
        }
    }

    void handleCheckoutScreen(sf::Event& event, sf::Vector2i mousePos) {
        if (payOnlineButton->isPressed(event, mousePos)) {
            selectedPaymentMethod = 1;
            showMessage("Online payment selected", sf::Color::Blue);
        }

        if (payCardButton->isPressed(event, mousePos)) {
            selectedPaymentMethod = 2;
            showMessage("Card payment selected", sf::Color::Blue);
        }

        if (payCashButton->isPressed(event, mousePos)) {
            selectedPaymentMethod = 3;
            showMessage("Cash payment selected", sf::Color::Blue);
        }

        if (confirmOrderButton->isPressed(event, mousePos)) {
            if (selectedPaymentMethod == 0) {
                showMessage("Please select a payment method!");
                return;
            }

            double total = currentStudent.getCartTotal();
            
            if (selectedPaymentMethod == 1) { // Online payment
                if (total > currentStudent.balance) {
                    showMessage("Insufficient balance!");
                    return;
                }
                currentStudent.balance -= total;
                currentStudent.updateBalanceInFile();
            }

            // Create order items
            vector<Item> orderItems;
            vector<int> orderQuantities;
            for (auto &cartItem : currentStudent.cart) {
                Item orderItem(cartItem.name, cartItem.price, cartItem.quantity, cartItem.isCustom);
                if (cartItem.isCustom) {
                    orderItem.ingredientUsage = cartItem.ingredientUsage;
                }
                orderItems.push_back(orderItem);
                orderQuantities.push_back(cartItem.quantity);
            }

            string paymentMethodStr;
            if (selectedPaymentMethod == 1) paymentMethodStr = "Online";
            else if (selectedPaymentMethod == 2) paymentMethodStr = "Card";
            else paymentMethodStr = "Cash";

            currentStudent.orderHistory.addOrder(orderItems, orderQuantities, total, 
                                                  paymentMethodStr, currentStudent.phoneNumber, 
                                                  currentStudent.username);

            showMessage("Order placed successfully! Your order will be ready in 1 minute.", sf::Color::Green);
            currentStudent.clearCart();
            currentScreen = MENU;
            selectedPaymentMethod = 0;
            
            // Save menu stock
            menu.saveStockToFile();
            ingredientInventory.saveToFile();
        }

        if (backToCartButton->isPressed(event, mousePos)) {
            currentScreen = CART;
            selectedPaymentMethod = 0;
        }
    }

    void handleOrdersScreen(sf::Event& event, sf::Vector2i mousePos) {
        if (viewAllOrdersButton->isPressed(event, mousePos)) {
            showAllOrders = true;
            showOngoingOrders = false;
            showCompletedOrders = false;
            showTrackOrder = false;
        }
        
        if (viewOngoingOrdersButton->isPressed(event, mousePos)) {
            showAllOrders = false;
            showOngoingOrders = true;
            showCompletedOrders = false;
            showTrackOrder = false;
        }
        
        if (viewCompletedOrdersButton->isPressed(event, mousePos)) {
            showAllOrders = false;
            showOngoingOrders = false;
            showCompletedOrders = true;
            showTrackOrder = false;
        }
        
        if (trackOrderButton->isPressed(event, mousePos)) {
            showAllOrders = false;
            showOngoingOrders = false;
            showCompletedOrders = false;
            showTrackOrder = true;
        }
        
        if (backToMenuFromOrders->isPressed(event, mousePos)) {
            currentScreen = MENU;
        }
        
        trackOrderIDBox->handleInput(event);
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                trackOrderIDBox->setActive(trackOrderIDBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
            }
        }
    }

    void handleCustomSandwichScreen(sf::Event& event, sf::Vector2i mousePos) {
    quantityInputBox->handleInput(event);
    
    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            quantityInputBox->setActive(quantityInputBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
        }
    }
    
    // Handle custom sandwich steps
    if (customSandwichStep == 0) { // Bread selection
        for (size_t i = 0; i < customSandwichButtonsStep1.size(); i++) {
            if (customSandwichButtonsStep1[i]->isPressed(event, mousePos)) {
                breadChoice = i + 1;
                showMessage("Selected " + string(i == 0 ? "White" : i == 1 ? "Brown" : "Multigrain") + " bread", sf::Color::Blue);
                customSandwichStep++;
                break;
            }
        }
    }
    else if (customSandwichStep == 1) { // Protein selection
        for (size_t i = 0; i < customSandwichButtonsStep2.size(); i++) {
            if (customSandwichButtonsStep2[i]->isPressed(event, mousePos)) {
                proteinChoice = i + 1;
                showMessage("Selected " + string(i == 0 ? "Chicken" : i == 1 ? "Beef" : i == 2 ? "Egg" : "No protein"), sf::Color::Blue);
                customSandwichStep++;
                break;
            }
        }
    }
    else if (customSandwichStep == 2) { // Veggies selection
        for (size_t i = 0; i < customSandwichButtonsStep3.size(); i++) {
            if (customSandwichButtonsStep3[i]->isPressed(event, mousePos)) {
                vegChoice = i + 1;
                showMessage("Selected " + string(i == 0 ? "Lettuce" : i == 1 ? "Cucumber" : i == 2 ? "Olives" : i == 3 ? "Onions" : "No veggies"), sf::Color::Blue);
                customSandwichStep++;
                break;
            }
        }
    }
    else if (customSandwichStep == 3) { // Sauce selection
        for (size_t i = 0; i < customSandwichButtonsStep4.size(); i++) {
            if (customSandwichButtonsStep4[i]->isPressed(event, mousePos)) {
                sauceChoice = i + 1;
                showMessage("Selected " + string(i == 0 ? "Mayo" : i == 1 ? "Garlic" : i == 2 ? "Chipotle" : "No sauce"), sf::Color::Blue);
                customSandwichStep++;
                break;
            }
        }
    }
    else if (customSandwichStep == 4) { // Extras selection
        for (size_t i = 0; i < customSandwichButtonsStep5.size(); i++) {
            if (customSandwichButtonsStep5[i]->isPressed(event, mousePos)) {
                extraChoice = i + 1;
                showMessage("Selected " + string(i == 0 ? "Cheese" : "No extras"), sf::Color::Blue);
                customSandwichStep++;
                break;
            }
        }
    }
    else if (customSandwichStep == 5) { // Quantity selection
        if (finishCustomButton->isPressed(event, mousePos)) {
            try {
                customQuantity = stoi(quantityInputBox->content);
                if (customQuantity <= 0) {
                    showMessage("Quantity must be positive!");
                    return;
                }
                
                // Build the sandwich
                currentCustomSandwich.buildSandwichGUI(breadChoice, proteinChoice, 
                                                      vegChoice, sauceChoice, extraChoice);
                
                // Check ingredient availability
                if (!ingredientInventory.hasEnoughAll(currentCustomSandwich.getChosenIngredients(), customQuantity)) {
                    showMessage("Not enough ingredients available!", sf::Color::Red);
                    return;
                }
                
                // Consume ingredients
                ingredientInventory.consumeAll(currentCustomSandwich.getChosenIngredients(), customQuantity);
                
                // Add to cart
                currentStudent.addToCart(
                    currentCustomSandwich.getName(),
                    currentCustomSandwich.getPrice(),
                    customQuantity,
                    true,
                    currentCustomSandwich.getIngredientUsage(customQuantity)
                );
                
                showMessage("Custom sandwich added to cart!", sf::Color::Green);
                currentScreen = BROWSE;
                selectedCategory = 0; // Savoury category
            } catch (...) {
                showMessage("Invalid quantity!");
            }
        }
    }
    
    if (prevStepButton->isPressed(event, mousePos) && customSandwichStep > 0) {
        customSandwichStep--;
        showMessage("Going back to previous step", sf::Color::Yellow);
    }
    
    if (nextStepButton->isPressed(event, mousePos) && customSandwichStep < 5) {
        // Validate current step before proceeding
        if (customSandwichStep == 0 && breadChoice == 0) {
            showMessage("Please select a bread type!", sf::Color::Red);
            return;
        }
        if (customSandwichStep == 1 && proteinChoice == 0) {
            showMessage("Please select a protein option!", sf::Color::Red);
            return;
        }
        if (customSandwichStep == 2 && vegChoice == 0) {
            showMessage("Please select a veggies option!", sf::Color::Red);
            return;
        }
        if (customSandwichStep == 3 && sauceChoice == 0) {
            showMessage("Please select a sauce option!", sf::Color::Red);
            return;
        }
        if (customSandwichStep == 4 && extraChoice == 0) {
            showMessage("Please select an extras option!", sf::Color::Red);
            return;
        }
        customSandwichStep++;
    }
    
    if (cancelCustomButton->isPressed(event, mousePos)) {
        currentScreen = BROWSE;
        selectedCategory = 0;
        showMessage("Custom sandwich cancelled", sf::Color::Yellow);
    }
}

    void handleRechargeScreen(sf::Event& event, sf::Vector2i mousePos) {
        rechargeAmountBox->handleInput(event);
        
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                rechargeAmountBox->setActive(rechargeAmountBox->box.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos)));
            }
        }

        if (rechargeConfirmButton->isPressed(event, mousePos)) {
            try {
                double amount = stod(rechargeAmountBox->content);
                if (amount > 0) {
                    currentStudent.balance += amount;
                    currentStudent.updateBalanceInFile();
                    showMessage("Recharged Rs." + to_string((int)amount) + " successfully!", sf::Color::Green);
                    rechargeAmountBox->clear();
                } else {
                    showMessage("Invalid amount!");
                }
            } catch (...) {
                showMessage("Invalid amount!");
            }
        }

        if (backToMenuFromRecharge->isPressed(event, mousePos)) {
            currentScreen = MENU;
        }
    }

    void drawLoginScreen() {
        sf::Text title("GrabNob Bakery System", font, 40);
        title.setFillColor(sf::Color::White);
        title.setPosition(200, 80);
        window.draw(title);
        
        loginIDBox->draw(window);
        loginPassBox->draw(window);
        loginButton->draw(window);
        registerNavButton->draw(window);
    }

    void drawRegisterScreen() {
        sf::Text title("Register New Account", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(250, 30);
        window.draw(title);
        
        regIDBox->draw(window);
        regUsernameBox->draw(window);
        regPassBox->draw(window);
        regPhoneBox->draw(window);
        regEmailBox->draw(window);
        regBalanceBox->draw(window);
        registerButton->draw(window);
        backToLoginButton->draw(window);
    }

    void drawMenuScreen() {
        sf::Text title("Main Menu", font, 40);
        title.setFillColor(sf::Color::White);
        title.setPosition(350, 80);
        window.draw(title);
        
        sf::Text welcomeText("Welcome, " + currentStudent.username, font, 24);
        welcomeText.setFillColor(sf::Color::Yellow);
        welcomeText.setPosition(350, 140);
        window.draw(welcomeText);
        
        sf::Text balanceText("Balance: Rs." + to_string((int)currentStudent.balance), font, 20);
        balanceText.setFillColor(sf::Color::Green);
        balanceText.setPosition(350, 170);
        window.draw(balanceText);
        
        for (auto btn : menuButtons) {
            btn->draw(window);
        }
        logoutButton->draw(window);
    }

    void drawBrowseScreen() {
        sf::Text title("Browse Menu", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(330, 30);
        window.draw(title);
        
        for (auto btn : categoryButtons) {
            btn->draw(window);
        }
        
        if (selectedCategory >= 0) {
            for (auto btn : itemButtons) {
                btn->draw(window);
            }
        }
        
        backToMenuButton->draw(window);
    }

    void drawCartScreen() {
        sf::Text title("Shopping Cart", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(330, 30);
        window.draw(title);

        if (currentStudent.cart.empty()) {
            sf::Text emptyText("Cart is empty!", font, 24);
            emptyText.setFillColor(sf::Color::White);
            emptyText.setPosition(350, 300);
            window.draw(emptyText);
        } else {
            for (auto btn : cartItemButtons) {
                btn->draw(window);
            }
            for (auto btn : removeButtons) {
                btn->draw(window);
            }

            sf::Text totalText("Total: Rs." + to_string((int)currentStudent.getCartTotal()), font, 24);
            totalText.setFillColor(sf::Color::Yellow);
            totalText.setPosition(100, 550);
            window.draw(totalText);
        }

        checkoutButton->draw(window);
        backToMenuFromCart->draw(window);
    }

    void drawCheckoutScreen() {
        sf::Text title("Checkout", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(370, 30);
        window.draw(title);

        sf::Text totalText("Total Amount: Rs." + to_string((int)currentStudent.getCartTotal()), font, 24);
        totalText.setFillColor(sf::Color::Yellow);
        totalText.setPosition(300, 150);
        window.draw(totalText);

        sf::Text balanceText("Your Balance: Rs." + to_string((int)currentStudent.balance), font, 20);
        balanceText.setFillColor(sf::Color::Green);
        balanceText.setPosition(300, 190);
        window.draw(balanceText);

        payOnlineButton->draw(window);
        payCardButton->draw(window);
        payCashButton->draw(window);

        // Highlight selected payment method
        if (selectedPaymentMethod == 1) {
            sf::RectangleShape highlight = payOnlineButton->shape;
            highlight.setOutlineThickness(3);
            highlight.setOutlineColor(sf::Color::Yellow);
            window.draw(highlight);
        } else if (selectedPaymentMethod == 2) {
            sf::RectangleShape highlight = payCardButton->shape;
            highlight.setOutlineThickness(3);
            highlight.setOutlineColor(sf::Color::Yellow);
            window.draw(highlight);
        } else if (selectedPaymentMethod == 3) {
            sf::RectangleShape highlight = payCashButton->shape;
            highlight.setOutlineThickness(3);
            highlight.setOutlineColor(sf::Color::Yellow);
            window.draw(highlight);
        }

        confirmOrderButton->draw(window);
        backToCartButton->draw(window);
    }

        void drawOrdersScreen() {
        sf::Text title("My Orders", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(350, 30);
        window.draw(title);

        if (showAllOrders) {
            sf::Text filterText("Showing: My Orders", font, 20); // Changed from "All Orders"
            filterText.setFillColor(sf::Color::Cyan);
            filterText.setPosition(300, 100);
            window.draw(filterText);
            
            // Get ONLY current user's orders
            vector<Order> userOrders = currentStudent.orderHistory.getOrdersForUser(currentStudent.username);
            
            if (userOrders.empty()) {
                sf::Text noOrdersText("No orders found!", font, 24);
                noOrdersText.setFillColor(sf::Color::White);
                noOrdersText.setPosition(350, 300);
                window.draw(noOrdersText);
            } else {
                int yPos = 150;
                for (size_t i = ordersScrollOffset; i < userOrders.size() && i < ordersScrollOffset + 8; i++) {
                    userOrders[i].updateStatus();
                    
                    sf::Text orderText(userOrders[i].getDisplayString(), font, 16);
                    orderText.setFillColor(userOrders[i].getTimeRemaining() > 0 ? sf::Color::Yellow : sf::Color::White);
                    orderText.setPosition(50, yPos);
                    window.draw(orderText);
                    
                    yPos += 60;
                }
            }
        }
        else if (showOngoingOrders) {
            sf::Text filterText("Showing: My Ongoing Orders", font, 20);
            filterText.setFillColor(sf::Color::Cyan);
            filterText.setPosition(300, 100);
            window.draw(filterText);
            
            // Get ONLY current user's ongoing orders
            vector<Order> ongoing = currentStudent.orderHistory.getOngoingOrdersForUser(currentStudent.username);
            
            if (ongoing.empty()) {
                sf::Text noOrdersText("No ongoing orders!", font, 24);
                noOrdersText.setFillColor(sf::Color::White);
                noOrdersText.setPosition(350, 300);
                window.draw(noOrdersText);
            } else {
                int yPos = 150;
                for (size_t i = 0; i < ongoing.size() && i < 8; i++) {
                    sf::Text orderText(ongoing[i].getDisplayString(), font, 16);
                    orderText.setFillColor(sf::Color::Yellow);
                    orderText.setPosition(50, yPos);
                    window.draw(orderText);
                    
                    // Draw timer
                    sf::Text timerText("Time: " + ongoing[i].getTimeRemainingString(), font, 14);
                    timerText.setFillColor(sf::Color::Red);
                    timerText.setPosition(650, yPos);
                    window.draw(timerText);
                    
                    yPos += 60;
                }
            }
        }
        else if (showCompletedOrders) {
            sf::Text filterText("Showing: My Completed Orders", font, 20);
            filterText.setFillColor(sf::Color::Cyan);
            filterText.setPosition(300, 100);
            window.draw(filterText);
            
            // Get ONLY current user's completed orders
            vector<Order> completed = currentStudent.orderHistory.getCompletedOrdersForUser(currentStudent.username);
            
            if (completed.empty()) {
                sf::Text noOrdersText("No completed orders!", font, 24);
                noOrdersText.setFillColor(sf::Color::White);
                noOrdersText.setPosition(350, 300);
                window.draw(noOrdersText);
            } else {
                int yPos = 150;
                for (size_t i = 0; i < completed.size() && i < 8; i++) {
                    sf::Text orderText(completed[i].getDisplayString(), font, 16);
                    orderText.setFillColor(sf::Color::White);
                    orderText.setPosition(50, yPos);
                    window.draw(orderText);
                    
                    yPos += 60;
                }
            }
        }
        else if (showTrackOrder) {
            sf::Text filterText("Track Order", font, 20);
            filterText.setFillColor(sf::Color::Cyan);
            filterText.setPosition(300, 100);
            window.draw(filterText);
            
            trackOrderIDBox->draw(window);
            
            if (!trackOrderIDBox->content.empty()) {
                Order* order = currentStudent.orderHistory.findOrderByID(trackOrderIDBox->content);
                if (order) {
                    // Check if this order belongs to the current user
                    if (order->username != currentStudent.username) {
                        sf::Text accessText("Access denied. This is not your order!", font, 20);
                        accessText.setFillColor(sf::Color::Red);
                        accessText.setPosition(50, 250);
                        window.draw(accessText);
                    } else {
                        order->updateStatus();
                        
                        sf::Text orderDetails(order->getDetailedString(), font, 14);
                        orderDetails.setFillColor(sf::Color::White);
                        orderDetails.setPosition(50, 250);
                        window.draw(orderDetails);
                        
                        // Draw timer for ongoing orders
                        if (order->getTimeRemaining() > 0) {
                            sf::Text timerText("Time Remaining: " + order->getTimeRemainingString(), font, 18);
                            timerText.setFillColor(sf::Color::Red);
                            timerText.setPosition(50, 500);
                            window.draw(timerText);
                        }
                    }
                } else {
                    sf::Text notFoundText("Order not found!", font, 20);
                    notFoundText.setFillColor(sf::Color::Red);
                    notFoundText.setPosition(50, 250);
                    window.draw(notFoundText);
                }
            }
        }

        viewAllOrdersButton->draw(window);
        viewOngoingOrdersButton->draw(window);
        viewCompletedOrdersButton->draw(window);
        trackOrderButton->draw(window);
        backToMenuFromOrders->draw(window);
    }

    void drawCustomSandwichScreen() {
    sf::Text title("Build Your Custom Sandwich", font, 35);
    title.setFillColor(sf::Color::White);
    title.setPosition(250, 30);
    window.draw(title);
    
    // Draw current step
    if (customSandwichStep < customStepLabels.size()) {
        sf::Text stepText(customStepLabels[customSandwichStep], font, 24);
        stepText.setFillColor(sf::Color::Yellow);
        stepText.setPosition(300, 100);
        window.draw(stepText);
    }
    
    // Draw buttons for current step
    if (customSandwichStep == 0) { // Bread selection
        for (auto btn : customSandwichButtonsStep1) {
            btn->draw(window);
        }
    }
    else if (customSandwichStep == 1) { // Protein selection
        for (auto btn : customSandwichButtonsStep2) {
            btn->draw(window);
        }
    }
    else if (customSandwichStep == 2) { // Veggies selection
        for (auto btn : customSandwichButtonsStep3) {
            btn->draw(window);
        }
    }
    else if (customSandwichStep == 3) { // Sauce selection
        for (auto btn : customSandwichButtonsStep4) {
            btn->draw(window);
        }
    }
    else if (customSandwichStep == 4) { // Extras selection
        for (auto btn : customSandwichButtonsStep5) {
            btn->draw(window);
        }
    }
    else if (customSandwichStep == 5) { // Quantity selection
        quantityInputBox->draw(window);
        finishCustomButton->draw(window);
        
        // Draw sandwich summary
        if (breadChoice > 0) {
            currentCustomSandwich.buildSandwichGUI(breadChoice, proteinChoice, vegChoice, sauceChoice, extraChoice);
            
            sf::Text summaryText(currentCustomSandwich.getSummary(), font, 16);
            summaryText.setFillColor(sf::Color::White);
            summaryText.setPosition(50, 200);
            window.draw(summaryText);
            
            // Draw ingredient stock
            sf::Text stockText(ingredientInventory.getStockDisplayText(), font, 14);
            stockText.setFillColor(sf::Color::Green);
            stockText.setPosition(50, 400);
            window.draw(stockText);
        }
    }
    
    // Draw navigation buttons
    if (customSandwichStep > 0) {
        prevStepButton->draw(window);
    }
    
    if (customSandwichStep < 5) {
        nextStepButton->draw(window);
    }
    
    cancelCustomButton->draw(window);
}

    void drawRechargeScreen() {
        sf::Text title("Recharge Balance", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(300, 80);
        window.draw(title);

        sf::Text balanceText("Current Balance: Rs." + to_string((int)currentStudent.balance), font, 22);
        balanceText.setFillColor(sf::Color::Green);
        balanceText.setPosition(300, 180);
        window.draw(balanceText);

        rechargeAmountBox->draw(window);
        rechargeConfirmButton->draw(window);
        backToMenuFromRecharge->draw(window);
    }

    void run() {
        while (window.isOpen()) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                
                switch (currentScreen) {
                    case LOGIN:
                        handleLoginScreen(event, mousePos);
                        break;
                    case REGISTER:
                        handleRegisterScreen(event, mousePos);
                        break;
                    case MENU:
                        handleMenuScreen(event, mousePos);
                        break;
                    case BROWSE:
                        handleBrowseScreen(event, mousePos);
                        break;
                    case CART:
                        handleCartScreen(event, mousePos);
                        break;
                    case CHECKOUT:
                        handleCheckoutScreen(event, mousePos);
                        break;
                    case ORDERS:
                        handleOrdersScreen(event, mousePos);
                        break;
                    case CUSTOM_SANDWICH:
                        handleCustomSandwichScreen(event, mousePos);
                        break;
                    case RECHARGE:
                        handleRechargeScreen(event, mousePos);
                        break;
                }
            }
            
            // Update buttons and input boxes
            switch (currentScreen) {
                case LOGIN:
                    loginButton->update(mousePos);
                    registerNavButton->update(mousePos);
                    loginIDBox->update();
                    loginPassBox->update();
                    break;
                case REGISTER:
                    registerButton->update(mousePos);
                    backToLoginButton->update(mousePos);
                    regIDBox->update();
                    regUsernameBox->update();
                    regPassBox->update();
                    regPhoneBox->update();
                    regEmailBox->update();
                    regBalanceBox->update();
                    break;
                case MENU:
                    for (auto btn : menuButtons) btn->update(mousePos);
                    logoutButton->update(mousePos);
                    break;
                case BROWSE:
                    for (auto btn : categoryButtons) btn->update(mousePos);
                    for (auto btn : itemButtons) btn->update(mousePos);
                    backToMenuButton->update(mousePos);
                    break;
                case CART:
                    for (auto btn : cartItemButtons) btn->update(mousePos);
                    for (auto btn : removeButtons) btn->update(mousePos);
                    checkoutButton->update(mousePos);
                    backToMenuFromCart->update(mousePos);
                    break;
                case CHECKOUT:
                    payOnlineButton->update(mousePos);
                    payCardButton->update(mousePos);
                    payCashButton->update(mousePos);
                    confirmOrderButton->update(mousePos);
                    backToCartButton->update(mousePos);
                    break;
                case ORDERS:
                    viewAllOrdersButton->update(mousePos);
                    viewOngoingOrdersButton->update(mousePos);
                    viewCompletedOrdersButton->update(mousePos);
                    trackOrderButton->update(mousePos);
                    backToMenuFromOrders->update(mousePos);
                    trackOrderIDBox->update();
                    break;
                case CUSTOM_SANDWICH:
                    if (customSandwichStep == 0) {
                        for (auto btn : customSandwichButtonsStep1) btn->update(mousePos);
                    } else if (customSandwichStep == 1) {
                        for (auto btn : customSandwichButtonsStep2) btn->update(mousePos);
                    } else if (customSandwichStep == 2) {
                        for (auto btn : customSandwichButtonsStep3) btn->update(mousePos);
                    } else if (customSandwichStep == 3) {
                        for (auto btn : customSandwichButtonsStep4) btn->update(mousePos);
                    } else if (customSandwichStep == 4) {
                        for (auto btn : customSandwichButtonsStep5) btn->update(mousePos);
                    }
                    nextStepButton->update(mousePos);
                    prevStepButton->update(mousePos);
                    finishCustomButton->update(mousePos);
                    cancelCustomButton->update(mousePos);
                    quantityInputBox->update();
                    break;
                case RECHARGE:
                    rechargeAmountBox->update();
                    rechargeConfirmButton->update(mousePos);
                    backToMenuFromRecharge->update(mousePos);
                    break;
            }
            
            // Clear and draw
            window.clear(sf::Color(30, 30, 50));
            
            switch (currentScreen) {
                case LOGIN:
                    drawLoginScreen();
                    break;
                case REGISTER:
                    drawRegisterScreen();
                    break;
                case MENU:
                    drawMenuScreen();
                    break;
                case BROWSE:
                    drawBrowseScreen();
                    break;
                case CART:
                    drawCartScreen();
                    break;
                case CHECKOUT:
                    drawCheckoutScreen();
                    break;
                case ORDERS:
                    drawOrdersScreen();
                    break;
                case CUSTOM_SANDWICH:
                    drawCustomSandwichScreen();
                    break;
                case RECHARGE:
                    drawRechargeScreen();
                    break;
            }
            
            // Draw message if active
            if (messageClock.getElapsedTime().asSeconds() < 3.0f) {
                window.draw(messageText);
            }
            
            window.display();
        }
    }
};

int main() {
    #ifdef _WIN32
    // Attach console for debugging
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    #endif

    cout << "=== GrabNob Bakery System Starting ===" << endl;
    cout << "Features included:" << endl;
    cout << "1. Real-time order timer (1 minute countdown)" << endl;
    cout << "2. Make Your Own Sandwich functionality" << endl;
    cout << "3. Ingredient inventory system" << endl;
    cout << "4. Stock persistence" << endl;
    cout << "================================" << endl;
    
    try {
        GrabNobGUI app;
        cout << "Starting main loop..." << endl;
        app.run();
        cout << "Program ended normally." << endl;
    }
    catch (const exception& e) {
        cout << "ERROR: " << e.what() << endl;
        system("pause");
        return 1;
    }
    catch (...) {
        cout << "ERROR: Unknown exception occurred!" << endl;
        system("pause");
        return 1;
    }
    
    return 0;
}