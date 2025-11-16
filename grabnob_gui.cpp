#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
using namespace sf;
using namespace std;

// Color theme
const Color DARK_GREEN(34, 139, 34);
const Color YELLOW(255, 215, 0);
const Color PURPLE(147, 112, 219);
const Color LIGHT_GREEN(144, 238, 144);
const Color WHITE(255, 255, 255);
const Color HOBNOB(65, 127, 104);

class LoginWindow {
private:
    RenderWindow window;
    Font font;
    
    // Text elements
    Text titleText;
    Text idLabel;
    Text passwordLabel;
    Text loginButton;
    Text registerButton;
    Text statusText;
    
    // Input boxes
    RectangleShape idBox;
    RectangleShape passwordBox;
    RectangleShape loginBtn;
    RectangleShape registerBtn;
    
    // Input strings
    string idInput;
    string passwordInput;
    Text idInputText;
    Text passwordInputText;
    
    bool idBoxActive;
    bool passwordBoxActive;

public:
    LoginWindow() : window(VideoMode(800, 600), "GrabNob - Login") {
        // Load font (you'll need to provide a font file)
        if (!font.loadFromFile("arial.ttf")) {
            cout << "Error loading font! Using default.\n";
            // SFML will use default font
        }
        
        idBoxActive = false;
        passwordBoxActive = false;
        
        setupUI();
    }
    
    void setupUI() {
        // Title
        titleText.setFont(font);
        titleText.setString("GrabNob");
        titleText.setCharacterSize(80);
        titleText.setFillColor(YELLOW);
        titleText.setStyle(Text::Bold);
        titleText.setPosition(250, 50);
        
        // ID Label
        idLabel.setFont(font);
        idLabel.setString("Student ID:");
        idLabel.setCharacterSize(24);
        idLabel.setFillColor(WHITE);
        idLabel.setPosition(250, 180);
        
        // Password Label
        passwordLabel.setFont(font);
        passwordLabel.setString("Password:");
        passwordLabel.setCharacterSize(24);
        passwordLabel.setFillColor(WHITE);
        passwordLabel.setPosition(250, 260);
        
        // ID Input Box
        idBox.setSize(Vector2f(350, 40));
        idBox.setPosition(250, 215);
        idBox.setFillColor(Color(50, 50, 50));
        idBox.setOutlineThickness(2);
        idBox.setOutlineColor(PURPLE);
        
        // Password Input Box
        passwordBox.setSize(Vector2f(350, 40));
        passwordBox.setPosition(250, 295);
        passwordBox.setFillColor(Color(50, 50, 50));
        passwordBox.setOutlineThickness(2);
        passwordBox.setOutlineColor(PURPLE);
        
        // Login Button
        loginBtn.setSize(Vector2f(160, 50));
        loginBtn.setPosition(250, 380);
        loginBtn.setFillColor(LIGHT_GREEN);
        
        loginButton.setFont(font);
        loginButton.setString("Login");
        loginButton.setCharacterSize(24);
        loginButton.setFillColor(Color::Black);
        loginButton.setPosition(300, 390);
        
        // Register Button
        registerBtn.setSize(Vector2f(160, 50));
        registerBtn.setPosition(440, 380);
        registerBtn.setFillColor(PURPLE);
        
        registerButton.setFont(font);
        registerButton.setString("Register");
        registerButton.setCharacterSize(24);
        registerButton.setFillColor(Color::Black);
        registerButton.setPosition(470, 390);
        
        // Input text displays
        idInputText.setFont(font);
        idInputText.setCharacterSize(20);
        idInputText.setFillColor(WHITE);
        idInputText.setPosition(250, 222);
        
        passwordInputText.setFont(font);
        passwordInputText.setCharacterSize(20);
        passwordInputText.setFillColor(WHITE);
        passwordInputText.setPosition(250, 302);
        
        // Status text
        statusText.setFont(font);
        statusText.setCharacterSize(18);
        statusText.setFillColor(YELLOW);
        statusText.setPosition(250, 480);
    }
    
    void handleEvent(Event& event) {
        if (event.type == Event::MouseButtonPressed) {
            Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            // Check if ID box clicked
            if (idBox.getGlobalBounds().contains(mousePos)) {
                idBoxActive = true;
                passwordBoxActive = false;
                idBox.setOutlineColor(YELLOW);
                passwordBox.setOutlineColor(PURPLE);
            }
            // Check if password box clicked
            else if (passwordBox.getGlobalBounds().contains(mousePos)) {
                passwordBoxActive = true;
                idBoxActive = false;
                passwordBox.setOutlineColor(YELLOW);
                idBox.setOutlineColor(PURPLE);
            }
            // Check if login button clicked
            else if (loginBtn.getGlobalBounds().contains(mousePos)) {
                handleLogin();
            }
            // Check if register button clicked
            else if (registerBtn.getGlobalBounds().contains(mousePos)) {
                handleRegister();
            }
            else {
                idBoxActive = false;
                passwordBoxActive = false;
                idBox.setOutlineColor(PURPLE);
                passwordBox.setOutlineColor(PURPLE);
            }
        }
        
        if (event.type == Event::TextEntered) {
            if (idBoxActive) {
                if (event.text.unicode == 8 && !idInput.empty()) { // Backspace
                    idInput.pop_back();
                } else if (event.text.unicode < 128 && event.text.unicode != 8) {
                    idInput += static_cast<char>(event.text.unicode);
                }
                idInputText.setString(idInput);
            }
            
            if (passwordBoxActive) {
                if (event.text.unicode == 8 && !passwordInput.empty()) { // Backspace
                    passwordInput.pop_back();
                } else if (event.text.unicode < 128 && event.text.unicode != 8) {
                    passwordInput += static_cast<char>(event.text.unicode);
                }
                // Display asterisks for password
                string maskedPassword(passwordInput.length(), '*');
                passwordInputText.setString(maskedPassword);
            }
        }
    }
    
    void handleLogin() {
        if (idInput.empty() || passwordInput.empty()) {
            statusText.setString("Please fill in all fields!");
            statusText.setFillColor(Color::Red);
        } else {
            statusText.setString("Login clicked! ID: " + idInput);
            statusText.setFillColor(LIGHT_GREEN);
            // Here you would integrate with your existing login logic
            cout << "Login attempt - ID: " << idInput << endl;
        }
    }
    
    void handleRegister() {
        statusText.setString("Register clicked! Opening registration...");
        statusText.setFillColor(LIGHT_GREEN);
        // Here you would integrate with your existing registration logic
        cout << "Register clicked" << endl;
    }
    
    void run() {
        while (window.isOpen()) {
            Event event;
            while (window.pollEvent(event)) {
                if (event.type == Event::Closed)
                    window.close();
                
                handleEvent(event);
            }
            
            // Clear window with dark background
            window.clear(Color(HOBNOB));
            
            // Draw all elements
            window.draw(titleText);
            window.draw(idLabel);
            window.draw(passwordLabel);
            window.draw(idBox);
            window.draw(passwordBox);
            window.draw(loginBtn);
            window.draw(registerBtn);
            window.draw(loginButton);
            window.draw(registerButton);
            window.draw(idInputText);
            window.draw(passwordInputText);
            window.draw(statusText);
            
            window.display();
        }
    }
};

int main() {
    LoginWindow loginWindow;
    loginWindow.run();
    return 0;
}