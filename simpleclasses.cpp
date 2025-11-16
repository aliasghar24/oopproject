#include <iostream>
#include <vector>
#include <string>
using namespace std;

class Item {
private:
    string name;
    double price;
    int quantity;   // stock available

public:
    Item(string n = "", double p = 0, int q = 0);
    string getName();
    double getPrice();
    int getQuantity();
    void setQuantity(int q);
    void displayItem();
};


class Category {
protected:
    string categoryName;
    vector<Item> items;  // has-a relationship with Item

public:
    Category(string name = "");
    virtual ~Category();
    void addItem(Item item);
    void displayItems();
    string getCategoryName();
    Item* getItemByName(string name);
};

class BakeryItems : public Category {
public:
    BakeryItems();
};

class Sweeteners : public Category {
public:
    Sweeteners();
};

class Savoury : public Category {
public:
    Savoury();
};

class Beverages : public Category {
public:
    Beverages();
};

class ShoppingCart {
private:
    vector<Item> items;   // has-a relationship with Item

public:
    void addItem(Item item);
    void removeItem(int index);
    double getTotal();
    void displayCart();
    void clearCart();
    bool isEmpty();
};

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

class OnlinePayment : public Payment {
private:
    string email;

public:
    OnlinePayment(double amt, string email);
    bool processPayment() override;
    void displayPaymentDetails() override;
};

class Balance {
private:
    double totalBalance;

public:
    Balance(double initial = 1000.0);
    double getBalance();
    bool deposit(double amount);
    bool withdraw(double amount);  // used when paying
};

class Student {
private:
    string id;
    string password;
    Balance balance;       // has-a
    ShoppingCart cart;     // has-a

public:
    Student();
    void inputDetails();
    bool login();
    void saveToFile();
    void updateBalanceInFile();
    void checkout();
};