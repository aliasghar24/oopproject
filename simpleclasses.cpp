#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
using namespace std;

// ==================== Item ====================
class Item {
private:
    string name;
    double price;
    int quantity;

public:
    Item(string n = "", double p = 0, int q = 5);
    string getName();
    double getPrice();
    int getQuantity();
    void setQuantity(int q);
};

// ==================== IngredientInventory ====================
class IngredientInventory {
private:
    unordered_map<string, int> stock;

public:
    IngredientInventory();
    void reset();
    bool hasEnough(const string &key, int qty);
    void consume(const string &key, int qty);
    void restore(const string &key, int qty);
    void displayForCustomBuilder();
};

// ==================== CustomSandwich ====================
class CustomSandwich {
private:
    string bread, protein, veggies, sauce, extras;
    double price;
    vector<string> chosenIngredientKeys;

public:
    CustomSandwich();
    Item buildSandwich();
    vector<string> getChosenIngredients();
};

// ==================== Category ====================
class Category {
protected:
    string categoryName;
    vector<Item> items;

public:
    Category(string name = "");
    virtual ~Category();
    void addItem(Item item);
    void displayCategory();
    string getCategoryName();
    Item* getItemRef(int index);
};

// ==================== Menu ====================
class Menu {
private:
    vector<Category> categories;

public:
    Menu();
    void displayCategories();
    void displayCategoryMenu(int categoryIndex);
    void resetStock();
    void increaseStockByName(const string &itemName, int qty);
    Item* getItemRef(int catIdx, int itemIdx);
};

// ==================== CartItem ====================
class CartItem {
public:
    string name;
    double price;
    int quantity;
    bool isCustom;
    vector<pair<string, int>> ingredientUsage;

    CartItem(string n = "", double p = 0, int q = 0);
};

// ==================== ShoppingCart ====================
class ShoppingCart {
private:
    vector<CartItem> items;

public:
    void addItem(const string &name, double price, int qty, bool isCustom = false, 
                 const vector<pair<string, int>> &usage = {});
    bool removeItem(int index, CartItem &removed);
    double getTotal();
    void displayCart();
    void clearCart();
    bool isEmpty();
    vector<CartItem> getItems();
};

// ==================== Order ====================
class Order {
private:
    string orderID;
    vector<CartItem> items;
    double totalAmount;
    time_t orderTime;
    int readyTimeSeconds;
    string status;
    string paymentMethod;
    string phoneNumber;

public:
    Order(string id, const vector<CartItem> &cartItems, double total, 
          string method, string phone);
    int getTimeRemaining();
    void updateStatus();
    void displayOrder();
    void displayOrderCompact();
    string getOrderID();
    string getStatus();
};

// ==================== OrderHistory ====================
class OrderHistory {
private:
    vector<Order> orders;
    int orderCounter;

public:
    OrderHistory();
    string generateOrderID();
    void addOrder(const vector<CartItem> &items, double total, 
                  string method, string phone);
    void displayOngoingOrders();
    void displayCompletedOrders();
    void displayAllOrders();
    void trackOrder(string ordID);
};

// ==================== Balance ====================
class Balance {
private:
    double totalBalance;

public:
    Balance(double initial = 1000);
    double getBalance();
    void displayBalance();
    bool deductAmount(double amount);
    void addAmount(double amount);
    bool rechargeBalance(double amount);
};

// ==================== Payment (Abstract) ====================
class Payment {
protected:
    double amount;
    string transactionID;

public:
    Payment(double amt);
    virtual ~Payment();
    virtual bool processPayment() = 0;
    virtual void displayPaymentDetails() = 0;
};

// ==================== OnlinePayment ====================
class OnlinePayment : public Payment {
private:
    string email;

public:
    OnlinePayment(double amt, string email);
    bool processPayment() override;
    void displayPaymentDetails() override;
};

// ==================== CardPayment ====================
class CardPayment : public Payment {
private:
    string cardNumber;

public:
    CardPayment(double amt, string card);
    bool processPayment() override;
    void displayPaymentDetails() override;
};

// ==================== CashPayment ====================
class CashPayment : public Payment {
public:
    CashPayment(double amt);
    bool processPayment() override;
    void displayPaymentDetails() override;
};

// ==================== Student ====================
class Student {
private:
    string id;
    string username;
    string password;
    string phoneNumber;
    string email;
    Balance balance;
    ShoppingCart cart;
    OrderHistory orderHistory;

public:
    Student();
    void inputDetails();
    void saveToFile();
    bool login();
    static bool deleteAccount();
    bool isValidPhoneNumber(string phone);
    bool isValidEmail(string email);
    void updateBalanceInFile();
    void checkout();
    
    // Getters
    string getID();
    string getUsername();
    string getEmail();
    string getPhoneNumber();
    Balance& getBalance();
    ShoppingCart& getCart();
    OrderHistory& getOrderHistory();
};