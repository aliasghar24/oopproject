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
};

// ===================== ORIGINAL CLASSES =====================
class Item {
public:
    string name;
    double price;
    int quantity;
    Item(string n = "", double p = 0, int q = 5) : name(n), price(p), quantity(q) {}
};

class CartItem {
public:
    string name;
    double price;
    int quantity;
    bool isCustom;

    CartItem(string n = "", double p = 0, int q = 0) : name(n), price(p), quantity(q), isCustom(false) {}
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

    void addOrder(vector<Item> items, vector<int> quantities, double total, string method, string phone, string username) {
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
            Item("Pizza Slice", 450, 5)
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

    void addToCart(const string &name, double price, int qty) {
        for (auto &ci : cart) {
            if (ci.name == name) {
                ci.quantity += qty;
                return;
            }
        }
        cart.push_back(CartItem(name, price, qty));
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
};

// ===================== GUI SCREENS =====================
enum Screen { LOGIN, REGISTER, MENU, BROWSE, CART, CHECKOUT, ORDERS, RECHARGE };

class GrabNobGUI {
private:
    sf::RenderWindow window;
    sf::Font font;
    Screen currentScreen;
    Student currentStudent;
    Menu menu;
    
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
    int selectedPaymentMethod; // 0=none, 1=online, 2=card, 3=cash
    
    // Orders Screen
    Button* viewAllOrdersButton;
    Button* viewCompletedOrdersButton;
    Button* backToMenuFromOrders;
    int ordersScrollOffset;
    
    // Recharge Screen
    InputBox* rechargeAmountBox;
    Button* rechargeConfirmButton;
    Button* backToMenuFromRecharge;
    
    sf::Text messageText;
    sf::Clock messageClock;
    string messageContent;

public:
    GrabNobGUI() : window(sf::VideoMode(900, 700), "GrabNob Bakery System"), 
               currentScreen(LOGIN), selectedCategory(-1), selectedPaymentMethod(0), ordersScrollOffset(0) {
    
    cout << "Constructor called..." << endl;
    window.setFramerateLimit(60);
    
    cout << "Attempting to load font..." << endl;
    
    // Try to load font from Windows fonts directory
    bool fontLoaded = false;
    
    if (font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
        cout << "Font loaded from C:\\Windows\\Fonts\\arial.ttf" << endl;
        fontLoaded = true;
    } else if (font.loadFromFile("arial.ttf")) {
        cout << "Font loaded from arial.ttf" << endl;
        fontLoaded = true;
    } else if (font.loadFromFile("C:\\Windows\\Fonts\\calibri.ttf")) {
        cout << "Font loaded from C:\\Windows\\Fonts\\calibri.ttf" << endl;
        fontLoaded = true;
    }
    
    if (!fontLoaded) {
        cout << "ERROR: Could not load any font file!" << endl;
        cout << "Please ensure arial.ttf exists in one of these locations:" << endl;
        cout << "1. C:\\Windows\\Fonts\\arial.ttf" << endl;
        cout << "2. Current directory (arial.ttf)" << endl;
        system("pause");
        exit(1);
    }
    
    cout << "Initializing UI components..." << endl;
    
    messageText.setFont(font);
    
    initLoginScreen();
    cout << "Login screen initialized" << endl;
    
    initRegisterScreen();
    cout << "Register screen initialized" << endl;
    
    initMenuScreen();
    cout << "Menu screen initialized" << endl;
    
    initBrowseScreen();
    cout << "Browse screen initialized" << endl;
    
    initCartScreen();
    cout << "Cart screen initialized" << endl;
    
    initCheckoutScreen();
    cout << "Checkout screen initialized" << endl;
    
    initOrdersScreen();
    cout << "Orders screen initialized" << endl;
    
    initRechargeScreen();
    cout << "Recharge screen initialized" << endl;
    
    messageText.setCharacterSize(16);
    messageText.setFillColor(sf::Color::Red);
    messageText.setPosition(50, 650);
    
    cout << "Constructor completed successfully!" << endl;
}

    ~GrabNobGUI() {
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
        delete viewCompletedOrdersButton;
        delete backToMenuFromOrders;
        
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

    void initOrdersScreen() {
        viewAllOrdersButton = new Button(300, 100, 300, 50, "View All Orders", font);
        viewCompletedOrdersButton = new Button(300, 170, 300, 50, "View Completed Orders", font);
        backToMenuFromOrders = new Button(300, 600, 300, 50, "Back to Menu", font);
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
            try {
                double balance = stod(regBalanceBox->content);
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
        
        if (menuButtons[2]->isPressed(event , mousePos)) {currentScreen = ORDERS;ordersScrollOffset = 0;}

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
            // Restore stock
            menu.increaseStockByName(currentStudent.cart[i].name, currentStudent.cart[i].quantity);
            
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

        // Create order
        vector<Item> orderItems;
        vector<int> orderQuantities;
        for (auto &item : currentStudent.cart) {
            orderItems.push_back(Item(item.name, item.price));
            orderQuantities.push_back(item.quantity);
        }

        string paymentMethodStr;
        if (selectedPaymentMethod == 1) paymentMethodStr = "Online";
        else if (selectedPaymentMethod == 2) paymentMethodStr = "Card";
        else paymentMethodStr = "Cash";

        currentStudent.orderHistory.addOrder(orderItems, orderQuantities, total, 
                                              paymentMethodStr, currentStudent.phoneNumber, 
                                              currentStudent.username);

        showMessage("Order placed successfully!", sf::Color::Green);
        currentStudent.clearCart();
        currentScreen = MENU;
        selectedPaymentMethod = 0;
    }

    if (backToCartButton->isPressed(event, mousePos)) {
        currentScreen = CART;
        selectedPaymentMethod = 0;
    }
}

void handleOrdersScreen(sf::Event& event, sf::Vector2i mousePos) {
    if (backToMenuFromOrders->isPressed(event, mousePos)) {
        currentScreen = MENU;
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
    
    sf::Text balanceText("Balance: Rs." + to_string((int)currentStudent.balance), font, 20);
    balanceText.setFillColor(sf::Color::Green);
    balanceText.setPosition(350, 140);
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

    vector<Order>& orders = currentStudent.orderHistory.getAllOrders();
    
    if (orders.empty()) {
        sf::Text emptyText("No orders yet!", font, 24);
        emptyText.setFillColor(sf::Color::White);
        emptyText.setPosition(350, 300);
        window.draw(emptyText);
    } else {
        int yPos = 250;
        for (size_t i = 0; i < orders.size() && i < 5; i++) {
            orders[i].updateStatus();
            
            stringstream ss;
            ss << "Order ID: " << orders[i].orderID << " | Total: Rs." << orders[i].totalAmount 
               << " | Status: " << orders[i].status;
            
            sf::Text orderText(ss.str(), font, 16);
            orderText.setFillColor(sf::Color::White);
            orderText.setPosition(50, yPos);
            window.draw(orderText);
            
            yPos += 70;
        }
    }

    backToMenuFromOrders->draw(window);
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
                backToMenuFromOrders->update(mousePos);
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
}};
int main() {
    #ifdef _WIN32
    // Attach console for debugging
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    #endif

    cout << "=== GrabNob Bakery System Starting ===" << endl;
    cout << "Current directory: " << endl;
    system("cd");
    
    cout << "\nAttempting to create GUI..." << endl;
    
    try {
        GrabNobGUI app;
        cout << "GUI created successfully!" << endl;
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