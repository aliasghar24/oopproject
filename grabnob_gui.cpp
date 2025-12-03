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

// ===================== ORIGINAL CLASSES (Simplified for GUI) =====================
class Item {
public:
    string name;
    double price;
    int quantity;
    Item(string n = "", double p = 0, int q = 5) : name(n), price(p), quantity(q) {}
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
};

class Student {
public:
    string id;
    string username;
    string password;
    string phoneNumber;
    string email;
    double balance;

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
};

// ===================== GUI SCREENS =====================
enum Screen { LOGIN, REGISTER, MENU, BROWSE };

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
    
    sf::Text messageText;
    sf::Clock messageClock;
    string messageContent;

public:
    GrabNobGUI() : window(sf::VideoMode(800, 600), "GrabNob Bakery System"), 
                   currentScreen(LOGIN), selectedCategory(-1) {
        window.setFramerateLimit(60);
        
        if (!font.loadFromFile("arial.ttf")) {
            // Try alternative font paths
            if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
                if (!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")) {
                    cout << "Error: Could not load font!" << endl;
                }
            }
        }
        
        messageText.setFont(font);
        
        initLoginScreen();
        initRegisterScreen();
        initMenuScreen();
        initBrowseScreen();
        
        messageText.setCharacterSize(16);
        messageText.setFillColor(sf::Color::Red);
        messageText.setPosition(50, 550);
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
    }

    void initLoginScreen() {
        loginIDBox = new InputBox(250, 200, 300, 40, "Student ID:", font);
        loginPassBox = new InputBox(250, 280, 300, 40, "Password:", font, true);
        loginButton = new Button(300, 360, 200, 50, "Login", font);
        registerNavButton = new Button(300, 430, 200, 50, "Register", font);
    }

    void initRegisterScreen() {
        regIDBox = new InputBox(250, 100, 300, 35, "Student ID:", font);
        regUsernameBox = new InputBox(250, 160, 300, 35, "Username:", font);
        regPassBox = new InputBox(250, 220, 300, 35, "Password:", font, true);
        regPhoneBox = new InputBox(250, 280, 300, 35, "Phone (11 digits):", font);
        regEmailBox = new InputBox(250, 340, 300, 35, "Email:", font);
        regBalanceBox = new InputBox(250, 400, 300, 35, "Initial Balance:", font);
        registerButton = new Button(250, 460, 140, 45, "Register", font);
        backToLoginButton = new Button(410, 460, 140, 45, "Back", font);
    }

    void initMenuScreen() {
        menuButtons.push_back(new Button(250, 200, 300, 60, "Browse Menu", font));
        menuButtons.push_back(new Button(250, 280, 300, 60, "View Cart", font));
        menuButtons.push_back(new Button(250, 360, 300, 60, "My Orders", font));
        logoutButton = new Button(250, 440, 300, 60, "Logout", font);
    }

    void initBrowseScreen() {
        for (size_t i = 0; i < 3; i++) {
            categoryButtons.push_back(new Button(50, 100 + i * 80, 200, 60, 
                menu.categoryNames[i], font));
        }
        backToMenuButton = new Button(50, 500, 200, 50, "Back to Menu", font);
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
            showMessage("Cart feature coming soon!", sf::Color::Blue);
        }
        
        if (menuButtons[2]->isPressed(event, mousePos)) {
            showMessage("Orders feature coming soon!", sf::Color::Blue);
        }
        
        if (logoutButton->isPressed(event, mousePos)) {
            currentScreen = LOGIN;
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
                    ss << menu.categories[i][j].name << " - Rs." << menu.categories[i][j].price;
                    itemButtons.push_back(new Button(300, 100 + j * 70, 450, 55, ss.str(), font));
                }
            }
        }
        
        for (size_t i = 0; i < itemButtons.size(); i++) {
            if (itemButtons[i]->isPressed(event, mousePos)) {
                showMessage("Added " + menu.categories[selectedCategory][i].name + " to cart!", sf::Color::Green);
            }
        }
        
        if (backToMenuButton->isPressed(event, mousePos)) {
            currentScreen = MENU;
            selectedCategory = -1;
            itemButtons.clear();
        }
    }

    void drawLoginScreen() {
        sf::Text title("GrabNob Bakery System", font, 40);
        title.setFillColor(sf::Color::White);
        title.setPosition(150, 80);
        window.draw(title);
        
        loginIDBox->draw(window);
        loginPassBox->draw(window);
        loginButton->draw(window);
        registerNavButton->draw(window);
    }

    void drawRegisterScreen() {
        sf::Text title("Register New Account", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(200, 30);
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
        title.setPosition(300, 80);
        window.draw(title);
        
        sf::Text balanceText("Balance: Rs." + to_string((int)currentStudent.balance), font, 20);
        balanceText.setFillColor(sf::Color::Green);
        balanceText.setPosition(300, 140);
        window.draw(balanceText);
        
        for (auto btn : menuButtons) {
            btn->draw(window);
        }
        logoutButton->draw(window);
    }

    void drawBrowseScreen() {
        sf::Text title("Browse Menu", font, 35);
        title.setFillColor(sf::Color::White);
        title.setPosition(280, 30);
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
                }
            }
            
            // Update buttons
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
    GrabNobGUI app;
    app.run();
    return 0;
}