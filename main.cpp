/**
 * ============================================================================
 * Project: Console Shopping System (Academic OOP Project)
 * Language: C++ (C++11/14/17 Compatible)
 * 
 * Key Object-Oriented Principles Demonstrated:
 *  1. Encapsulation: Strict data hiding with private fields and validated getters/setters.
 *  2. Composition: Cart owns and manages a dynamic collection of CartItems and Products.
 *  3. Polymorphism & Inheritance: Abstract Payment base class with concrete UPIPayment,
 *     CardPayment, and CashOnDeliveryPayment implementations.
 *  4. Abstraction: Clean separation between data models, storage/catalog, and UI controller.
 * 
 * UI Constraints:
 *  - Maximum 3 ANSI Colors:
 *      * WHITE  (\033[97m): General text, prompts, tables, and descriptions.
 *      * ORANGE (\033[38;5;208m): Headers, borders, frames, and accent menus.
 *      * GREEN  (\033[92m): Prices, grand totals, and success confirmations.
 *  - Strict fixed-width column alignment using std::setw, std::left, and std::right.
 * ============================================================================
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <algorithm>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#endif

// ============================================================================
// 1. ANSI COLOR SYSTEM & TERMINAL CONFIGURATION
// Strictly limited to NO MORE THAN 3 COLORS: White, Orange, and Green.
// ============================================================================
namespace UI {
    // 3 Allowed Colors + Reset
    const std::string RESET  = "\033[0m";
    const std::string WHITE  = "\033[97m";          // General text, prompts, table text
    const std::string ORANGE = "\033[38;5;208m";     // Headers, borders, dividers, menus
    const std::string GREEN  = "\033[92m";          // Prices, totals, confirmations

    // Enable Virtual Terminal Processing on Windows so ANSI codes render properly
    inline void enableVirtualTerminal() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
        }
#endif
    }

    // Helper to format currency consistently (e.g. "$129.99")
    inline std::string formatPrice(double price) {
        std::ostringstream ss;
        ss << "$" << std::fixed << std::setprecision(2) << price;
        return ss.str();
    }

    // Centered string helper for table headers
    inline std::string center(const std::string& text, int width) {
        int pad = width - static_cast<int>(text.length());
        if (pad <= 0) return text.substr(0, width);
        int padLeft = pad / 2;
        int padRight = pad - padLeft;
        return std::string(padLeft, ' ') + text + std::string(padRight, ' ');
    }
}

// ============================================================================
// 2. CORE DOMAIN CLASSES: Product, CartItem, Cart
// ============================================================================

/**
 * @class Product
 * @brief Represents an individual product in the store catalog.
 * Demonstrates: Encapsulation with private state and safe accessors.
 */
class Product {
private:
    int id;
    std::string name;
    std::string category;
    double price;
    int stock;

public:
    // Constructor
    Product(int id, const std::string& name, const std::string& category, double price, int stock)
        : id(id), name(name), category(category), price(price), stock(stock) {}

    // Getters
    int getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    // Setters / Stock modifiers with validation
    bool reduceStock(int quantity) {
        if (quantity > 0 && quantity <= stock) {
            stock -= quantity;
            return true;
        }
        return false;
    }

    void restock(int quantity) {
        if (quantity > 0) {
            stock += quantity;
        }
    }
};

/**
 * @class CartItem
 * @brief Represents a product along with the desired purchase quantity.
 * Demonstrates: Encapsulation and composition helper for the Cart.
 */
class CartItem {
private:
    Product product;
    int quantity;

public:
    CartItem(const Product& product, int quantity)
        : product(product), quantity(quantity) {}

    // Accessors
    const Product& getProduct() const { return product; }
    int getQuantity() const { return quantity; }

    void setQuantity(int qty) {
        if (qty > 0) {
            quantity = qty;
        }
    }

    void addQuantity(int qty) {
        if (qty > 0) {
            quantity += qty;
        }
    }

    double getTotalPrice() const {
        return product.getPrice() * quantity;
    }
};

/**
 * @class Cart
 * @brief Manages a dynamic collection of items selected by the customer.
 * Demonstrates: Composition (Cart HAS-A collection of CartItems), aggregation.
 */
class Cart {
private:
    std::vector<CartItem> items;

public:
    // Adds an item or updates quantity if already in the cart
    void addItem(const Product& product, int quantity) {
        for (auto& item : items) {
            if (item.getProduct().getId() == product.getId()) {
                item.addQuantity(quantity);
                return;
            }
        }
        items.emplace_back(product, quantity);
    }

    // Updates quantity of a specific item
    bool updateItemQuantity(int productId, int newQuantity) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getProduct().getId() == productId) {
                if (newQuantity <= 0) {
                    items.erase(it);
                    return true;
                } else {
                    it->setQuantity(newQuantity);
                    return true;
                }
            }
        }
        return false;
    }

    // Removes an item completely from the cart
    bool removeItem(int productId) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getProduct().getId() == productId) {
                items.erase(it);
                return true;
            }
        }
        return false;
    }

    // Clear cart contents
    void clear() {
        items.clear();
    }

    // Queries
    bool isEmpty() const {
        return items.empty();
    }

    size_t getItemCount() const {
        return items.size();
    }

    int getTotalUnits() const {
        int total = 0;
        for (const auto& item : items) {
            total += item.getQuantity();
        }
        return total;
    }

    double calculateGrandTotal() const {
        double total = 0.0;
        for (const auto& item : items) {
            total += item.getTotalPrice();
        }
        return total;
    }

    const std::vector<CartItem>& getItems() const {
        return items;
    }

    const CartItem* findItem(int productId) const {
        for (const auto& item : items) {
            if (item.getProduct().getId() == productId) {
                return &item;
            }
        }
        return nullptr;
    }
};

// ============================================================================
// 3. POLYMORPHIC PAYMENT SYSTEM
// Demonstrates: Abstract Base Class, Pure Virtual Functions, and Dynamic Polymorphism.
// ============================================================================

/**
 * @class Payment
 * @brief Abstract Base Class for payment methods.
 */
class Payment {
protected:
    std::string transactionId;
    std::string timestamp;

    std::string generateTransactionId(const std::string& prefix) {
        std::ostringstream ss;
        ss << prefix << "-" << (100000 + rand() % 900000);
        return ss.str();
    }

    std::string getCurrentTimestamp() {
        std::time_t now = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

public:
    virtual ~Payment() = default;

    // Pure virtual method that every payment gateway must implement
    virtual bool processPayment(double amount) = 0;

    // Informational virtual accessors
    virtual std::string getMethodName() const = 0;
    virtual std::string getDetails() const = 0;

    const std::string& getTransactionId() const { return transactionId; }
    const std::string& getTimestamp() const { return timestamp; }
};

/**
 * @class UPIPayment
 * @brief Handles Unified Payments Interface (UPI) transactions.
 */
class UPIPayment : public Payment {
private:
    std::string upiId;

public:
    bool processPayment(double amount) override {
        std::cout << UI::WHITE << "\n  Enter UPI ID (e.g. user@okhdfcbank / user@paytm): " << UI::RESET;
        std::getline(std::cin, upiId);

        // Simple validation
        if (upiId.find('@') == std::string::npos || upiId.length() < 4) {
            std::cout << UI::WHITE << "  [!] Invalid UPI ID format. Payment rejected.\n" << UI::RESET;
            return false;
        }

        std::cout << UI::WHITE << "  Connecting to UPI Gateway for " 
                  << UI::GREEN << UI::formatPrice(amount) << UI::WHITE << "...\n";
        std::cout << UI::WHITE << "  Please enter 4 or 6 digit UPI PIN: " << UI::RESET;
        std::string pin;
        std::getline(std::cin, pin);

        if (pin.length() < 4 || pin.length() > 6) {
            std::cout << UI::WHITE << "  [!] Invalid PIN entered. Transaction aborted.\n" << UI::RESET;
            return false;
        }

        transactionId = generateTransactionId("UPI");
        timestamp = getCurrentTimestamp();

        std::cout << UI::GREEN << "  [SUCCESS] Payment verified and received via UPI!\n" << UI::RESET;
        return true;
    }

    std::string getMethodName() const override {
        return "UPI Payment";
    }

    std::string getDetails() const override {
        return "VPA: " + upiId;
    }
};

/**
 * @class CardPayment
 * @brief Handles Debit and Credit card transactions with masked representation.
 */
class CardPayment : public Payment {
private:
    std::string cardHolder;
    std::string maskedCard;

public:
    bool processPayment(double amount) override {
        std::cout << UI::WHITE << "\n  Enter Cardholder Name: " << UI::RESET;
        std::getline(std::cin, cardHolder);

        std::cout << UI::WHITE << "  Enter 16-Digit Card Number: " << UI::RESET;
        std::string rawCard;
        std::getline(std::cin, rawCard);

        // Remove spaces or hyphens if entered
        std::string cleanCard;
        for (char ch : rawCard) {
            if (ch >= '0' && ch <= '9') cleanCard += ch;
        }

        if (cleanCard.length() != 16) {
            std::cout << UI::WHITE << "  [!] Invalid Card Number length. Must be 16 digits.\n" << UI::RESET;
            return false;
        }

        std::cout << UI::WHITE << "  Enter Expiry Date (MM/YY): " << UI::RESET;
        std::string expiry;
        std::getline(std::cin, expiry);

        std::cout << UI::WHITE << "  Enter 3-Digit CVV: " << UI::RESET;
        std::string cvv;
        std::getline(std::cin, cvv);

        if (cvv.length() != 3) {
            std::cout << UI::WHITE << "  [!] Invalid CVV security code.\n" << UI::RESET;
            return false;
        }

        // Mask card number (e.g., ****-****-****-1234)
        maskedCard = "****-****-****-" + cleanCard.substr(12, 4);
        transactionId = generateTransactionId("CARD");
        timestamp = getCurrentTimestamp();

        std::cout << UI::WHITE << "  Authorizing with card network for " 
                  << UI::GREEN << UI::formatPrice(amount) << UI::WHITE << "...\n";
        std::cout << UI::GREEN << "  [SUCCESS] Card transaction approved by issuing bank!\n" << UI::RESET;
        return true;
    }

    std::string getMethodName() const override {
        return "Credit / Debit Card";
    }

    std::string getDetails() const override {
        return cardHolder + " (" + maskedCard + ")";
    }
};

/**
 * @class CashOnDeliveryPayment
 * @brief Handles Cash on Delivery with phone and address verification.
 */
class CashOnDeliveryPayment : public Payment {
private:
    std::string address;
    std::string phone;

public:
    bool processPayment(double amount) override {
        (void)amount;
        std::cout << UI::WHITE << "\n  Enter Shipping Address: " << UI::RESET;
        std::getline(std::cin, address);

        std::cout << UI::WHITE << "  Enter Contact Phone Number: " << UI::RESET;
        std::getline(std::cin, phone);

        if (address.empty() || phone.length() < 7) {
            std::cout << UI::WHITE << "  [!] Incomplete delivery details for Cash on Delivery.\n" << UI::RESET;
            return false;
        }

        transactionId = generateTransactionId("COD");
        timestamp = getCurrentTimestamp();

        std::cout << UI::GREEN << "  [SUCCESS] Cash on Delivery order verified and scheduled for dispatch!\n" << UI::RESET;
        return true;
    }

    std::string getMethodName() const override {
        return "Cash on Delivery (COD)";
    }

    std::string getDetails() const override {
        return "Deliver to: " + address + " | Contact: " + phone;
    }
};

// ============================================================================
// 4. PRODUCT CATALOG / INVENTORY
// ============================================================================

/**
 * @class Catalog
 * @brief Holds and manages store inventory.
 */
class Catalog {
private:
    std::vector<Product> products;

public:
    Catalog() {
        // Pre-populate with realistic inventory items
        products.emplace_back(101, "Logitech MX Master 3S Mouse", "Electronics",  99.99, 15);
        products.emplace_back(102, "Keychron Q1 Pro Mechanical Keyboard", "Electronics", 199.50,  8);
        products.emplace_back(103, "Dell UltraSharp 27\" 4K Monitor", "Electronics", 449.00,  5);
        products.emplace_back(104, "Sony WH-1000XM5 Headphones",   "Audio",       348.00, 10);
        products.emplace_back(105, "Anker 737 GaN 140W Power Bank", "Accessories",  89.95, 20);
        products.emplace_back(106, "SanDisk 1TB Extreme Portable SSD", "Storage", 119.99, 14);
        products.emplace_back(107, "Ergonomic Aluminium Laptop Stand", "Furniture", 45.00, 25);
        products.emplace_back(108, "USB-C Braided Thunderbolt 4 Cable", "Accessories", 24.99, 40);
    }

    Product* findProductById(int id) {
        for (auto& p : products) {
            if (p.getId() == id) {
                return &p;
            }
        }
        return nullptr;
    }

    const std::vector<Product>& getAllProducts() const {
        return products;
    }
};

// ============================================================================
// 5. APPLICATION CONTROLLER & USER INTERFACE
// ============================================================================

/**
 * @class ShoppingSystem
 * @brief High-level controller driving user interaction and business logic.
 */
class ShoppingSystem {
private:
    Catalog catalog;
    Cart cart;

    // Helper for safe integer input to prevent stream corruption
    int getSafeInt(const std::string& prompt) {
        int value;
        while (true) {
            std::cout << UI::WHITE << prompt << UI::RESET;
            if (std::cin >> value) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return value;
            } else {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << UI::WHITE << "  [!] Invalid input. Please enter a valid number.\n" << UI::RESET;
            }
        }
    }

    // Helper for safe string input
    std::string getSafeLine(const std::string& prompt) {
        std::cout << UI::WHITE << prompt << UI::RESET;
        std::string s;
        std::getline(std::cin, s);
        return s;
    }

    void pausePrompt() {
        std::cout << UI::WHITE << "\n  Press [Enter] to return to menu..." << UI::RESET;
        std::cin.get();
    }

public:
    ShoppingSystem() {
        UI::enableVirtualTerminal();
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    // Main execution loop
    void run() {
        bool running = true;
        while (running) {
            displayMainMenu();
            int choice = getSafeInt("  Enter your choice [1-7]: ");

            switch (choice) {
                case 1:
                    displayCatalogTable();
                    pausePrompt();
                    break;
                case 2:
                    handleAddToCart();
                    pausePrompt();
                    break;
                case 3:
                    displayCartSummary();
                    pausePrompt();
                    break;
                case 4:
                    handleUpdateCart();
                    pausePrompt();
                    break;
                case 5:
                    handleClearCart();
                    pausePrompt();
                    break;
                case 6:
                    handleCheckout();
                    pausePrompt();
                    break;
                case 7:
                    displayExitBanner();
                    running = false;
                    break;
                default:
                    std::cout << UI::WHITE << "\n  [!] Option not recognized. Please choose between 1 and 7.\n" << UI::RESET;
                    pausePrompt();
                    break;
            }
        }
    }

private:
    // UI: Main Menu
    void displayMainMenu() {
        std::cout << "\n";
        std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n";
        std::cout << "|                       CONSTELLATION SHOPPING HUB                       |\n";
        std::cout << "|                 Object-Oriented Academic C++ Showcase                  |\n";
        std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;

        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "1. Browse Product Catalog" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "2. Add Product to Cart" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "3. View Cart Summary & Subtotals" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "4. Modify Item Quantity / Remove from Cart" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "5. Clear Entire Shopping Cart" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "6. Proceed to Checkout & Polymorphic Payment" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) 
                  << "7. Exit Application" << UI::ORANGE << " |\n";

        std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n";
        std::cout << "| Current Cart: " << UI::GREEN << std::left << std::setw(3) << cart.getTotalUnits() 
                  << UI::WHITE << " items | Grand Total: " << UI::GREEN 
                  << std::left << std::setw(15) << UI::formatPrice(cart.calculateGrandTotal())
                  << UI::ORANGE << "                 |\n";
        std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;
    }

    // UI: Product Catalog Table
    void displayCatalogTable() {
        std::cout << "\n";
        std::cout << UI::ORANGE << "+------+------------------------------------+---------------+------------+-------+\n";
        std::cout << "|  ID  | Product Name                       | Category      | Price      | Stock |\n";
        std::cout << "+------+------------------------------------+---------------+------------+-------+\n" << UI::RESET;

        for (const auto& p : catalog.getAllProducts()) {
            std::cout << UI::ORANGE << "| " << UI::WHITE << std::left  << std::setw(4)  << p.getId()
                      << UI::ORANGE << " | " << UI::WHITE << std::left  << std::setw(34) << p.getName()
                      << UI::ORANGE << " | " << UI::WHITE << std::left  << std::setw(13) << p.getCategory()
                      << UI::ORANGE << " | " << UI::GREEN << std::right << std::setw(10) << UI::formatPrice(p.getPrice())
                      << UI::ORANGE << " | " << UI::WHITE << std::right << std::setw(5)  << p.getStock()
                      << UI::ORANGE << " |\n";
        }

        std::cout << UI::ORANGE << "+------+------------------------------------+---------------+------------+-------+\n" << UI::RESET;
    }

    // Action: Add To Cart
    void handleAddToCart() {
        displayCatalogTable();

        int prodId = getSafeInt("\n  Enter Product ID to add: ");
        Product* product = catalog.findProductById(prodId);

        if (!product) {
            std::cout << UI::WHITE << "  [!] Product with ID " << prodId << " does not exist.\n" << UI::RESET;
            return;
        }

        if (product->getStock() <= 0) {
            std::cout << UI::WHITE << "  [!] Sorry, '" << product->getName() << "' is currently out of stock.\n" << UI::RESET;
            return;
        }

        int currentInCart = 0;
        const CartItem* existingItem = cart.findItem(prodId);
        if (existingItem) {
            currentInCart = existingItem->getQuantity();
        }

        int availableToAdd = product->getStock() - currentInCart;
        if (availableToAdd <= 0) {
            std::cout << UI::WHITE << "  [!] You already have the maximum available stock (" 
                      << product->getStock() << ") in your cart.\n" << UI::RESET;
            return;
        }

        std::cout << UI::WHITE << "  Available to add: " << availableToAdd << " unit(s).\n";
        int qty = getSafeInt("  Enter desired quantity: ");

        if (qty <= 0) {
            std::cout << UI::WHITE << "  [!] Quantity must be greater than zero.\n" << UI::RESET;
            return;
        }

        if (qty > availableToAdd) {
            std::cout << UI::WHITE << "  [!] Cannot add " << qty << " units. Only " 
                      << availableToAdd << " more available in inventory.\n" << UI::RESET;
            return;
        }

        cart.addItem(*product, qty);
        std::cout << UI::GREEN << "\n  [SUCCESS] Added " << qty << "x '" << product->getName() 
                  << "' to your cart!\n" << UI::RESET;
    }

    // UI: Cart Summary Table
    void displayCartSummary() {
        if (cart.isEmpty()) {
            std::cout << "\n";
            std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n";
            std::cout << "|                          SHOPPING CART STATUS                          |\n";
            std::cout << "+------------------------------------------------------------------------+\n";
            std::cout << "| " << UI::WHITE << std::left << std::setw(70) << "Your shopping cart is currently empty." << UI::ORANGE << " |\n";
            std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;
            return;
        }

        std::cout << "\n";
        std::cout << UI::ORANGE << "+------+------------------------------------+------------+-----+------------+\n";
        std::cout << "|  ID  | Product Name                       | Unit Price | Qty | Total Cost |\n";
        std::cout << "+------+------------------------------------+------------+-----+------------+\n" << UI::RESET;

        for (const auto& item : cart.getItems()) {
            std::cout << UI::ORANGE << "| " << UI::WHITE << std::left  << std::setw(4)  << item.getProduct().getId()
                      << UI::ORANGE << " | " << UI::WHITE << std::left  << std::setw(34) << item.getProduct().getName()
                      << UI::ORANGE << " | " << UI::GREEN << std::right << std::setw(10) << UI::formatPrice(item.getProduct().getPrice())
                      << UI::ORANGE << " | " << UI::WHITE << std::right << std::setw(3)  << item.getQuantity()
                      << UI::ORANGE << " | " << UI::GREEN << std::right << std::setw(10) << UI::formatPrice(item.getTotalPrice())
                      << UI::ORANGE << " |\n";
        }

        std::cout << UI::ORANGE << "+------+------------------------------------+------------+-----+------------+\n";
        std::cout << "| " << UI::WHITE << std::left << std::setw(50) << "Total Items: " + std::to_string(cart.getTotalUnits())
                  << UI::ORANGE << " | GRAND TOTAL: " << UI::GREEN << std::right << std::setw(11) 
                  << UI::formatPrice(cart.calculateGrandTotal()) << UI::ORANGE << " |\n";
        std::cout << "+--------------------------------------------------+---------------------+\n" << UI::RESET;
    }

    // Action: Update or Remove Cart Item
    void handleUpdateCart() {
        if (cart.isEmpty()) {
            std::cout << UI::WHITE << "\n  [!] Your cart is empty. Nothing to modify.\n" << UI::RESET;
            return;
        }

        displayCartSummary();

        int prodId = getSafeInt("\n  Enter Product ID to modify (or 0 to cancel): ");
        if (prodId == 0) return;

        const CartItem* item = cart.findItem(prodId);
        if (!item) {
            std::cout << UI::WHITE << "  [!] Product ID " << prodId << " is not in your cart.\n" << UI::RESET;
            return;
        }

        Product* product = catalog.findProductById(prodId);
        std::cout << UI::WHITE << "  Selected: '" << item->getProduct().getName() 
                  << "' (Current quantity: " << item->getQuantity() << ")\n";
        std::cout << UI::WHITE << "  Available Stock: " << product->getStock() << "\n";
        std::cout << UI::WHITE << "  (Tip: Set quantity to 0 to remove item from cart)\n";

        int newQty = getSafeInt("  Enter new quantity: ");
        if (newQty < 0) {
            std::cout << UI::WHITE << "  [!] Quantity cannot be negative.\n" << UI::RESET;
            return;
        }

        if (newQty > product->getStock()) {
            std::cout << UI::WHITE << "  [!] Cannot set quantity to " << newQty 
                      << ". Only " << product->getStock() << " available in inventory.\n" << UI::RESET;
            return;
        }

        if (newQty == 0) {
            cart.removeItem(prodId);
            std::cout << UI::GREEN << "  [SUCCESS] Removed '" << product->getName() 
                      << "' from cart.\n" << UI::RESET;
        } else {
            cart.updateItemQuantity(prodId, newQty);
            std::cout << UI::GREEN << "  [SUCCESS] Updated quantity of '" << product->getName() 
                      << "' to " << newQty << ".\n" << UI::RESET;
        }
    }

    // Action: Clear Cart
    void handleClearCart() {
        if (cart.isEmpty()) {
            std::cout << UI::WHITE << "\n  [!] Your cart is already empty.\n" << UI::RESET;
            return;
        }

        std::string confirm = getSafeLine("\n  Are you sure you want to clear your cart? (y/n): ");
        if (confirm == "y" || confirm == "Y") {
            cart.clear();
            std::cout << UI::GREEN << "  [SUCCESS] Cart has been cleared.\n" << UI::RESET;
        } else {
            std::cout << UI::WHITE << "  Clear cart action canceled.\n" << UI::RESET;
        }
    }

    // Action: Checkout & Polymorphic Payment
    void handleCheckout() {
        if (cart.isEmpty()) {
            std::cout << UI::WHITE << "\n  [!] Your cart is empty. Add products before checking out.\n" << UI::RESET;
            return;
        }

        // Verify that stock is still available for all items in the cart
        for (const auto& item : cart.getItems()) {
            Product* p = catalog.findProductById(item.getProduct().getId());
            if (!p || p->getStock() < item.getQuantity()) {
                std::cout << UI::WHITE << "\n  [!] Stock check failed for '" 
                          << item.getProduct().getName() << "'. Requested: " 
                          << item.getQuantity() << ", Available: " 
                          << (p ? p->getStock() : 0) << ".\n";
                std::cout << "  Please adjust your cart before checking out.\n" << UI::RESET;
                return;
            }
        }

        displayCartSummary();

        double grandTotal = cart.calculateGrandTotal();
        std::cout << "\n";
        std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n";
        std::cout << "|                       SELECT PAYMENT GATEWAY                           |\n";
        std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) << "1. Unified Payments Interface (UPI)" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) << "2. Credit / Debit Card Gateway" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) << "3. Cash on Delivery (COD)" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << std::left << std::setw(70) << "4. Cancel & Return to Main Menu" << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n" << UI::RESET;

        int payChoice = getSafeInt("  Select payment method [1-4]: ");
        if (payChoice == 4) {
            std::cout << UI::WHITE << "  Checkout canceled.\n" << UI::RESET;
            return;
        }

        // Polymorphic Payment Pointer demonstrating runtime polymorphism
        std::unique_ptr<Payment> paymentGateway;

        switch (payChoice) {
            case 1:
                paymentGateway = std::unique_ptr<Payment>(new UPIPayment());
                break;
            case 2:
                paymentGateway = std::unique_ptr<Payment>(new CardPayment());
                break;
            case 3:
                paymentGateway = std::unique_ptr<Payment>(new CashOnDeliveryPayment());
                break;
            default:
                std::cout << UI::WHITE << "  [!] Invalid payment option selected.\n" << UI::RESET;
                return;
        }

        // Execute polymorphic payment
        bool paymentSuccess = paymentGateway->processPayment(grandTotal);

        if (!paymentSuccess) {
            std::cout << UI::WHITE << "\n  [!] Checkout failed: Payment could not be completed.\n" << UI::RESET;
            return;
        }

        // Deduct inventory stock for purchased products
        for (const auto& item : cart.getItems()) {
            Product* p = catalog.findProductById(item.getProduct().getId());
            if (p) {
                p->reduceStock(item.getQuantity());
            }
        }

        // Print Order Receipt
        printReceipt(*paymentGateway, grandTotal);

        // Clear cart upon successful order
        cart.clear();
    }

    // UI: Formatted Receipt Printing
    void printReceipt(const Payment& payment, double grandTotal) {
        std::cout << "\n";
        std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n";
        std::cout << "|                           TAX INVOICE / RECEIPT                        |\n";
        std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;
        std::cout << UI::ORANGE << "| " << UI::WHITE << "Transaction Ref: " << std::left << std::setw(51) 
                  << payment.getTransactionId() << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << "Date & Time    : " << std::left << std::setw(51) 
                  << payment.getTimestamp() << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << "Payment Method : " << std::left << std::setw(51) 
                  << payment.getMethodName() << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "| " << UI::WHITE << "Account Details: " << std::left << std::setw(51) 
                  << payment.getDetails().substr(0, 51) << UI::ORANGE << " |\n";
        std::cout << UI::ORANGE << "+------+------------------------------------+------------+-----+------------+\n";
        std::cout << "|  ID  | Item Description                   | Unit Price | Qty | Subtotal   |\n";
        std::cout << "+------+------------------------------------+------------+-----+------------+\n" << UI::RESET;

        for (const auto& item : cart.getItems()) {
            std::cout << UI::ORANGE << "| " << UI::WHITE << std::left  << std::setw(4)  << item.getProduct().getId()
                      << UI::ORANGE << " | " << UI::WHITE << std::left  << std::setw(34) << item.getProduct().getName()
                      << UI::ORANGE << " | " << UI::GREEN << std::right << std::setw(10) << UI::formatPrice(item.getProduct().getPrice())
                      << UI::ORANGE << " | " << UI::WHITE << std::right << std::setw(3)  << item.getQuantity()
                      << UI::ORANGE << " | " << UI::GREEN << std::right << std::setw(10) << UI::formatPrice(item.getTotalPrice())
                      << UI::ORANGE << " |\n";
        }

        std::cout << UI::ORANGE << "+------+------------------------------------+------------+-----+------------+\n";
        std::cout << "| " << UI::WHITE << std::left << std::setw(50) << "Total Items Billed: " + std::to_string(cart.getTotalUnits())
                  << UI::ORANGE << " | AMOUNT PAID: " << UI::GREEN << std::right << std::setw(10) 
                  << UI::formatPrice(grandTotal) << UI::ORANGE << " |\n";
        std::cout << "+--------------------------------------------------+---------------------+\n";
        std::cout << "| " << UI::GREEN << std::left << std::setw(70) 
                  << "Thank you for shopping with Constellation Hub! Order Confirmed." << UI::ORANGE << " |\n";
        std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;
    }

    void displayExitBanner() {
        std::cout << "\n";
        std::cout << UI::ORANGE << "+------------------------------------------------------------------------+\n";
        std::cout << "|               THANK YOU FOR VISITING CONSTELLATION HUB                 |\n";
        std::cout << "|                    Have a productive day ahead!                        |\n";
        std::cout << "+------------------------------------------------------------------------+\n" << UI::RESET;
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================
int main() {
    ShoppingSystem app;
    app.run();
    return 0;
}
