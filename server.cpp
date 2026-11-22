/**
 * ============================================================================
 * Project: TechHouse - E-Commerce System & Admin Portal
 * Component: Native C++ Localhost HTTP Web Server (Indian Rupee ₹ Edition)
 * Target: Windows (Winsock2) - http://localhost:8080
 * ============================================================================
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <map>

#pragma comment(lib, "ws2_32.lib")

// ============================================================================
// 1. CORE DOMAIN CLASSES
// ============================================================================

class Product {
private:
    int id;
    std::string name;
    std::string category;
    double price;
    double originalPrice;
    int stock;
    std::string image;
    double rating;
    int reviewCount;

public:
    Product(int id, const std::string& name, const std::string& category, 
            double price, double originalPrice, int stock, const std::string& image = "",
            double rating = 4.5, int reviewCount = 850)
        : id(id), name(name), category(category), price(price), 
          originalPrice(originalPrice > 0 ? originalPrice : price * 1.35), 
          stock(stock), image(image), rating(rating), reviewCount(reviewCount) {}

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::string& getCategory() const { return category; }
    double getPrice() const { return price; }
    double getOriginalPrice() const { return originalPrice; }
    int getStock() const { return stock; }
    const std::string& getImage() const { return image; }
    double getRating() const { return rating; }
    int getReviewCount() const { return reviewCount; }

    void setName(const std::string& n) { name = n; }
    void setCategory(const std::string& c) { category = c; }
    void setPrice(double p) { if (p >= 0.0) price = p; }
    void setOriginalPrice(double op) { if (op >= 0.0) originalPrice = op; }
    void setStock(int s) { if (s >= 0) stock = s; }
    void setImage(const std::string& img) { image = img; }

    bool reduceStock(int quantity) {
        if (quantity > 0 && quantity <= stock) {
            stock -= quantity;
            return true;
        }
        return false;
    }

    void restock(int quantity) {
        if (quantity > 0) stock += quantity;
    }
};

class CartItem {
private:
    Product product;
    int quantity;

public:
    CartItem(const Product& product, int quantity)
        : product(product), quantity(quantity) {}

    const Product& getProduct() const { return product; }
    int getQuantity() const { return quantity; }

    void setQuantity(int qty) { if (qty > 0) quantity = qty; }
    void addQuantity(int qty) { if (qty > 0) quantity += qty; }
    double getTotalPrice() const { return product.getPrice() * quantity; }
};

class Cart {
private:
    std::vector<CartItem> items;

public:
    void addItem(const Product& product, int quantity) {
        for (auto& item : items) {
            if (item.getProduct().getId() == product.getId()) {
                item.addQuantity(quantity);
                return;
            }
        }
        items.emplace_back(product, quantity);
    }

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

    bool removeItem(int productId) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->getProduct().getId() == productId) {
                items.erase(it);
                return true;
            }
        }
        return false;
    }

    void clear() {
        items.clear();
    }

    bool isEmpty() const { return items.empty(); }
    size_t getItemCount() const { return items.size(); }

    int getTotalUnits() const {
        int total = 0;
        for (const auto& item : items) total += item.getQuantity();
        return total;
    }

    double calculateGrandTotal() const {
        double total = 0.0;
        for (const auto& item : items) total += item.getTotalPrice();
        return total;
    }

    double calculateTotalSavings() const {
        double totalSavings = 0.0;
        for (const auto& item : items) {
            double orig = item.getProduct().getOriginalPrice();
            double cur = item.getProduct().getPrice();
            if (orig > cur) {
                totalSavings += (orig - cur) * item.getQuantity();
            }
        }
        return totalSavings;
    }

    const std::vector<CartItem>& getItems() const { return items; }

    const CartItem* findItem(int productId) const {
        for (const auto& item : items) {
            if (item.getProduct().getId() == productId) return &item;
        }
        return nullptr;
    }
};

// ============================================================================
// 2. USER AUTHENTICATION & ROLES
// ============================================================================

class User {
private:
    std::string username;
    std::string password;
    std::string fullName;
    std::string role; // "admin" or "customer"

public:
    User() = default;
    User(const std::string& username, const std::string& password, const std::string& fullName, const std::string& role)
        : username(username), password(password), fullName(fullName), role(role) {}

    const std::string& getUsername() const { return username; }
    const std::string& getFullName() const { return fullName; }
    const std::string& getRole() const { return role; }

    bool verifyPassword(const std::string& pwd) const {
        return password == pwd;
    }
};

class UserManager {
private:
    std::map<std::string, User> users;

public:
    UserManager() {
        users["admin"] = User("admin", "admin123", "Store Administrator", "admin");
        users["user"]  = User("user",  "pass123",  "Aarav Sharma",        "customer");
    }

    bool registerUser(const std::string& username, const std::string& password, 
                      const std::string& fullName, const std::string& role) {
        if (username.empty() || password.empty()) return false;
        if (users.find(username) != users.end()) return false;

        users[username] = User(username, password, fullName, role.empty() ? "customer" : role);
        return true;
    }

    const User* authenticate(const std::string& username, const std::string& password) const {
        auto it = users.find(username);
        if (it != users.end() && it->second.verifyPassword(password)) {
            return &(it->second);
        }
        return nullptr;
    }
};

// ============================================================================
// 3. POLYMORPHIC PAYMENT SYSTEM
// ============================================================================

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
    virtual bool processPayment(double amount) = 0;
    virtual std::string getMethodName() const = 0;
    virtual std::string getDetails() const = 0;

    const std::string& getTransactionId() const { return transactionId; }
    const std::string& getTimestamp() const { return timestamp; }
};

class UPIPayment : public Payment {
private:
    std::string upiId;
    std::string pin;

public:
    UPIPayment(const std::string& upiId, const std::string& pin)
        : upiId(upiId), pin(pin) {}

    bool processPayment(double amount) override {
        (void)amount;
        if (upiId.find('@') == std::string::npos || upiId.length() < 4) return false;
        if (pin.length() < 4 || pin.length() > 6) return false;

        transactionId = generateTransactionId("UPI-IN");
        timestamp = getCurrentTimestamp();
        return true;
    }

    std::string getMethodName() const override { return "UPI Payment (GPay / PhonePe / Paytm)"; }
    std::string getDetails() const override { return "VPA: " + upiId; }
};

class CardPayment : public Payment {
private:
    std::string cardHolder;
    std::string cardNumber;
    std::string maskedCard;
    std::string expiry;
    std::string cvv;

public:
    CardPayment(const std::string& holder, const std::string& number, const std::string& expiry, const std::string& cvv)
        : cardHolder(holder), cardNumber(number), expiry(expiry), cvv(cvv) {}

    bool processPayment(double amount) override {
        (void)amount;
        if (cardHolder.empty() || cardNumber.length() != 16 || cvv.length() != 3) return false;

        maskedCard = "****-****-****-" + cardNumber.substr(12, 4);
        transactionId = generateTransactionId("CARD-IN");
        timestamp = getCurrentTimestamp();
        return true;
    }

    std::string getMethodName() const override { return "RuPay / Visa / MasterCard"; }
    std::string getDetails() const override { return cardHolder + " (" + maskedCard + ")"; }
};

class CashOnDeliveryPayment : public Payment {
private:
    std::string address;
    std::string phone;

public:
    CashOnDeliveryPayment(const std::string& address, const std::string& phone)
        : address(address), phone(phone) {}

    bool processPayment(double amount) override {
        (void)amount;
        if (address.length() < 5 || phone.length() < 7) return false;

        transactionId = generateTransactionId("COD-IN");
        timestamp = getCurrentTimestamp();
        return true;
    }

    std::string getMethodName() const override { return "Cash on Delivery (Cash / UPI at Doorstep)"; }
    std::string getDetails() const override { return "Deliver to: " + address + " | Ph: " + phone; }
};

class Catalog {
private:
    std::vector<Product> products;
    int nextId;

public:
    Catalog() : nextId(109) {
        // Pre-populated with realistic Indian Rupee (₹) prices and MRPs
        products.emplace_back(101, "Logitech MX Master 3S Wireless Mouse", "Electronics", 7995.0, 10995.0, 15, "mouse", 4.7, 3420);
        products.emplace_back(102, "Keychron Q1 Pro Wireless Mechanical Keyboard", "Electronics", 16499.0, 21999.0, 8, "keyboard", 4.8, 1280);
        products.emplace_back(103, "Dell UltraSharp 27\" 4K UHD USB-C Monitor", "Electronics", 38990.0, 52000.0, 5, "monitor", 4.6, 840);
        products.emplace_back(104, "Sony WH-1000XM5 Noise Cancelling Headphones", "Audio", 26990.0, 34990.0, 10, "headphones", 4.8, 5910);
        products.emplace_back(105, "Anker 737 GaN 140W Fast Charger Power Bank", "Accessories", 7499.0, 9999.0, 20, "powerbank", 4.5, 2150);
        products.emplace_back(106, "SanDisk 1TB Extreme Portable NVMe SSD", "Storage", 9890.0, 14500.0, 14, "ssd", 4.6, 4280);
        products.emplace_back(107, "Ergonomic Aluminium Laptop & Tablet Stand", "Furniture", 2499.0, 3999.0, 25, "stand", 4.4, 1890);
        products.emplace_back(108, "USB-C Braided 40Gbps Thunderbolt 4 Cable", "Accessories", 1499.0, 2499.0, 40, "cable", 4.5, 960);
    }

    Product* findProductById(int id) {
        for (auto& p : products) {
            if (p.getId() == id) return &p;
        }
        return nullptr;
    }

    const std::vector<Product>& getAllProducts() const { return products; }

    bool addProduct(const std::string& name, const std::string& category, double price, double originalPrice, int stock, const std::string& image) {
        if (name.empty() || price < 0.0 || stock < 0) return false;
        double orig = originalPrice > 0 ? originalPrice : price * 1.35;
        products.emplace_back(nextId++, name, category, price, orig, stock, image.empty() ? "gadget" : image, 4.5, 50);
        return true;
    }

    bool updateProduct(int id, double price, double originalPrice, int stock) {
        Product* p = findProductById(id);
        if (!p) return false;
        p->setPrice(price);
        if (originalPrice > 0) p->setOriginalPrice(originalPrice);
        p->setStock(stock);
        return true;
    }

    bool deleteProduct(int id) {
        for (auto it = products.begin(); it != products.end(); ++it) {
            if (it->getId() == id) {
                products.erase(it);
                return true;
            }
        }
        return false;
    }
};

// ============================================================================
// 4. JSON UTILITIES
// ============================================================================

namespace JsonUtil {
    std::string escape(const std::string& s) {
        std::ostringstream o;
        for (char c : s) {
            if (c == '"') o << "\\\"";
            else if (c == '\\') o << "\\\\";
            else if (c == '\b') o << "\\b";
            else if (c == '\f') o << "\\f";
            else if (c == '\n') o << "\\n";
            else if (c == '\r') o << "\\r";
            else if (c == '\t') o << "\\t";
            else o << c;
        }
        return o.str();
    }

    std::string parseString(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";
        pos = json.find(':', pos);
        if (pos == std::string::npos) return "";
        pos = json.find('"', pos);
        if (pos == std::string::npos) return "";
        size_t endPos = json.find('"', pos + 1);
        if (endPos == std::string::npos) return "";
        return json.substr(pos + 1, endPos - pos - 1);
    }

    int parseInt(const std::string& json, const std::string& key, int defaultVal = 0) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return defaultVal;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return defaultVal;
        while (pos < json.length() && (json[pos] == ':' || json[pos] == ' ' || json[pos] == '\t')) pos++;
        size_t endPos = pos;
        while (endPos < json.length() && (isdigit(json[endPos]) || json[endPos] == '-')) endPos++;
        if (endPos > pos) {
            return std::atoi(json.substr(pos, endPos - pos).c_str());
        }
        return defaultVal;
    }

    double parseDouble(const std::string& json, const std::string& key, double defaultVal = 0.0) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return defaultVal;
        pos = json.find(':', pos);
        if (pos == std::string::npos) return defaultVal;
        while (pos < json.length() && (json[pos] == ':' || json[pos] == ' ' || json[pos] == '\t')) pos++;
        size_t endPos = pos;
        while (endPos < json.length() && (isdigit(json[endPos]) || json[endPos] == '.' || json[endPos] == '-')) endPos++;
        if (endPos > pos) {
            return std::atof(json.substr(pos, endPos - pos).c_str());
        }
        return defaultVal;
    }
}

// ============================================================================
// 5. HTTP SERVER & ROUTING ENGINE
// ============================================================================

class LocalhostHttpServer {
private:
    int port;
    SOCKET listenSocket;
    Catalog catalog;
    Cart cart;
    UserManager userManager;
    double totalRevenue;

    std::string readFileContent(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) return "";
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    std::string getMimeType(const std::string& path) {
        if (path.rfind(".html") != std::string::npos) return "text/html; charset=utf-8";
        if (path.rfind(".css") != std::string::npos) return "text/css; charset=utf-8";
        if (path.rfind(".js") != std::string::npos) return "application/javascript; charset=utf-8";
        if (path.rfind(".json") != std::string::npos) return "application/json; charset=utf-8";
        if (path.rfind(".png") != std::string::npos) return "image/png";
        if (path.rfind(".svg") != std::string::npos) return "image/svg+xml";
        return "text/plain";
    }

    std::string serializeProductsJson() {
        std::ostringstream ss;
        ss << "[";
        const auto& prods = catalog.getAllProducts();
        for (size_t i = 0; i < prods.size(); ++i) {
            const auto& p = prods[i];
            ss << "{"
               << "\"id\":" << p.getId() << ","
               << "\"name\":\"" << JsonUtil::escape(p.getName()) << "\","
               << "\"category\":\"" << JsonUtil::escape(p.getCategory()) << "\","
               << "\"price\":" << std::fixed << std::setprecision(2) << p.getPrice() << ","
               << "\"originalPrice\":" << std::fixed << std::setprecision(2) << p.getOriginalPrice() << ","
               << "\"stock\":" << p.getStock() << ","
               << "\"image\":\"" << JsonUtil::escape(p.getImage()) << "\","
               << "\"rating\":" << std::fixed << std::setprecision(1) << p.getRating() << ","
               << "\"reviewCount\":" << p.getReviewCount()
               << "}";
            if (i + 1 < prods.size()) ss << ",";
        }
        ss << "]";
        return ss.str();
    }

    std::string serializeCartJson() {
        std::ostringstream ss;
        ss << "{"
           << "\"itemCount\":" << cart.getItemCount() << ","
           << "\"totalUnits\":" << cart.getTotalUnits() << ","
           << "\"grandTotal\":" << std::fixed << std::setprecision(2) << cart.calculateGrandTotal() << ","
           << "\"totalSavings\":" << std::fixed << std::setprecision(2) << cart.calculateTotalSavings() << ","
           << "\"items\":[";
        const auto& items = cart.getItems();
        for (size_t i = 0; i < items.size(); ++i) {
            const auto& it = items[i];
            ss << "{"
               << "\"id\":" << it.getProduct().getId() << ","
               << "\"name\":\"" << JsonUtil::escape(it.getProduct().getName()) << "\","
               << "\"price\":" << std::fixed << std::setprecision(2) << it.getProduct().getPrice() << ","
               << "\"originalPrice\":" << std::fixed << std::setprecision(2) << it.getProduct().getOriginalPrice() << ","
               << "\"quantity\":" << it.getQuantity() << ","
               << "\"total\":" << std::fixed << std::setprecision(2) << it.getTotalPrice()
               << "}";
            if (i + 1 < items.size()) ss << ",";
        }
        ss << "]}";
        return ss.str();
    }

    void sendHttpResponse(SOCKET clientSocket, int statusCode, const std::string& statusText, 
                          const std::string& contentType, const std::string& body) {
        std::ostringstream resp;
        resp << "HTTP/1.1 " << statusCode << " " << statusText << "\r\n";
        resp << "Content-Type: " << contentType << "\r\n";
        resp << "Content-Length: " << body.length() << "\r\n";
        resp << "Access-Control-Allow-Origin: *\r\n";
        resp << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
        resp << "Access-Control-Allow-Headers: Content-Type\r\n";
        resp << "Connection: close\r\n";
        resp << "\r\n";
        resp << body;

        std::string responseStr = resp.str();
        send(clientSocket, responseStr.c_str(), static_cast<int>(responseStr.length()), 0);
    }

    void handleClient(SOCKET clientSocket) {
        char buffer[16384];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            closesocket(clientSocket);
            return;
        }
        buffer[bytesRead] = '\0';
        std::string request(buffer, bytesRead);

        // Parse Request Line
        std::istringstream reqStream(request);
        std::string method, path, httpVer;
        reqStream >> method >> path >> httpVer;

        // Extract Request Body (for POST requests)
        std::string body;
        size_t headerEnd = request.find("\r\n\r\n");
        size_t headerLen = 4;
        if (headerEnd == std::string::npos) {
            headerEnd = request.find("\n\n");
            headerLen = 2;
        }

        if (headerEnd != std::string::npos) {
            body = request.substr(headerEnd + headerLen);
        }

        // Check Content-Length to ensure full payload is received
        size_t clPos = request.find("Content-Length:");
        if (clPos == std::string::npos) clPos = request.find("content-length:");
        if (clPos != std::string::npos) {
            size_t valStart = request.find(':', clPos) + 1;
            while (valStart < request.length() && (request[valStart] == ' ' || request[valStart] == '\t')) valStart++;
            size_t valEnd = request.find("\r\n", valStart);
            if (valEnd == std::string::npos) valEnd = request.find("\n", valStart);
            if (valEnd != std::string::npos) {
                int contentLen = std::atoi(request.substr(valStart, valEnd - valStart).c_str());
                while (static_cast<int>(body.length()) < contentLen) {
                    char extraBuf[4096];
                    int extraRead = recv(clientSocket, extraBuf, sizeof(extraBuf) - 1, 0);
                    if (extraRead <= 0) break;
                    body.append(extraBuf, extraRead);
                }
            }
        }

        std::cout << "  [" << method << "] " << path << std::endl;

        if (method == "OPTIONS") {
            sendHttpResponse(clientSocket, 204, "No Content", "text/plain", "");
            closesocket(clientSocket);
            return;
        }

        // ==========================================
        // AUTHENTICATION ENDPOINTS
        // ==========================================
        if (method == "POST" && path == "/api/auth/register") {
            std::string username = JsonUtil::parseString(body, "username");
            std::string password = JsonUtil::parseString(body, "password");
            std::string fullName = JsonUtil::parseString(body, "fullName");
            std::string role     = JsonUtil::parseString(body, "role");

            if (username.empty() || password.empty()) {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"success\":false,\"error\":\"Username and password required\"}");
            } else if (userManager.registerUser(username, password, fullName.empty() ? username : fullName, role)) {
                sendHttpResponse(clientSocket, 200, "OK", "application/json", "{\"success\":true,\"message\":\"Account registered successfully\"}");
            } else {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"success\":false,\"error\":\"Username already exists\"}");
            }
        }
        else if (method == "POST" && path == "/api/auth/login") {
            std::string username = JsonUtil::parseString(body, "username");
            std::string password = JsonUtil::parseString(body, "password");

            const User* u = userManager.authenticate(username, password);
            if (u) {
                std::ostringstream ss;
                ss << "{\"success\":true,\"user\":{"
                   << "\"username\":\"" << JsonUtil::escape(u->getUsername()) << "\","
                   << "\"fullName\":\"" << JsonUtil::escape(u->getFullName()) << "\","
                   << "\"role\":\"" << JsonUtil::escape(u->getRole()) << "\""
                   << "}}";
                sendHttpResponse(clientSocket, 200, "OK", "application/json", ss.str());
            } else {
                sendHttpResponse(clientSocket, 401, "Unauthorized", "application/json", "{\"success\":false,\"error\":\"Invalid credentials\"}");
            }
        }

        // ==========================================
        // ADMIN MANAGEMENT ENDPOINTS
        // ==========================================
        else if (method == "GET" && path == "/api/admin/stats") {
            int totalProducts = static_cast<int>(catalog.getAllProducts().size());
            int totalStockUnits = 0;
            int lowStockCount = 0;
            for (const auto& p : catalog.getAllProducts()) {
                totalStockUnits += p.getStock();
                if (p.getStock() <= 5) lowStockCount++;
            }

            std::ostringstream ss;
            ss << "{"
               << "\"totalProducts\":" << totalProducts << ","
               << "\"totalStockUnits\":" << totalStockUnits << ","
               << "\"lowStockCount\":" << lowStockCount << ","
               << "\"totalRevenue\":" << std::fixed << std::setprecision(2) << totalRevenue
               << "}";
            sendHttpResponse(clientSocket, 200, "OK", "application/json", ss.str());
        }
        else if (method == "POST" && path == "/api/admin/products/add") {
            std::string name         = JsonUtil::parseString(body, "name");
            std::string category     = JsonUtil::parseString(body, "category");
            double price             = JsonUtil::parseDouble(body, "price", 0.0);
            double originalPrice     = JsonUtil::parseDouble(body, "originalPrice", 0.0);
            int stock                = JsonUtil::parseInt(body, "stock", 0);
            std::string image        = JsonUtil::parseString(body, "image");

            if (catalog.addProduct(name, category, price, originalPrice, stock, image)) {
                sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeProductsJson());
            } else {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"error\":\"Failed to create product\"}");
            }
        }
        else if (method == "POST" && path == "/api/admin/products/update") {
            int id                   = JsonUtil::parseInt(body, "id");
            double price             = JsonUtil::parseDouble(body, "price", 0.0);
            double originalPrice     = JsonUtil::parseDouble(body, "originalPrice", 0.0);
            int stock                = JsonUtil::parseInt(body, "stock", 0);

            if (catalog.updateProduct(id, price, originalPrice, stock)) {
                sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeProductsJson());
            } else {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"error\":\"Product not found\"}");
            }
        }
        else if (method == "POST" && path == "/api/admin/products/delete") {
            int id = JsonUtil::parseInt(body, "id");
            if (catalog.deleteProduct(id)) {
                cart.removeItem(id);
                sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeProductsJson());
            } else {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"error\":\"Product not found\"}");
            }
        }

        // ==========================================
        // PUBLIC STOREFRONT ENDPOINTS
        // ==========================================
        else if (method == "GET" && path == "/api/products") {
            sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeProductsJson());
        }
        else if (method == "GET" && path == "/api/cart") {
            sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeCartJson());
        }
        else if (method == "POST" && path == "/api/cart/add") {
            int prodId = JsonUtil::parseInt(body, "productId");
            int qty = JsonUtil::parseInt(body, "quantity", 1);

            Product* p = catalog.findProductById(prodId);
            if (!p) {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"error\":\"Product not found\"}");
            } else {
                int inCart = 0;
                const CartItem* item = cart.findItem(prodId);
                if (item) inCart = item->getQuantity();

                if (inCart + qty > p->getStock()) {
                    sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"error\":\"Insufficient stock available\"}");
                } else {
                    cart.addItem(*p, qty);
                    sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeCartJson());
                }
            }
        }
        else if (method == "POST" && path == "/api/cart/update") {
            int prodId = JsonUtil::parseInt(body, "productId");
            int qty = JsonUtil::parseInt(body, "quantity", 1);

            Product* p = catalog.findProductById(prodId);
            if (p && qty > p->getStock()) {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"error\":\"Quantity exceeds available stock\"}");
            } else {
                cart.updateItemQuantity(prodId, qty);
                sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeCartJson());
            }
        }
        else if (method == "POST" && path == "/api/cart/remove") {
            int prodId = JsonUtil::parseInt(body, "productId");
            cart.removeItem(prodId);
            sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeCartJson());
        }
        else if (method == "POST" && path == "/api/cart/clear") {
            cart.clear();
            sendHttpResponse(clientSocket, 200, "OK", "application/json", serializeCartJson());
        }
        else if (method == "POST" && path == "/api/checkout") {
            if (cart.isEmpty()) {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"success\":false,\"error\":\"Cart is empty\"}");
                closesocket(clientSocket);
                return;
            }

            for (const auto& item : cart.getItems()) {
                Product* p = catalog.findProductById(item.getProduct().getId());
                if (!p || p->getStock() < item.getQuantity()) {
                    sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"success\":false,\"error\":\"Stock error for " + item.getProduct().getName() + "\"}");
                    closesocket(clientSocket);
                    return;
                }
            }

            std::string payMethod = JsonUtil::parseString(body, "paymentMethod");
            std::unique_ptr<Payment> gateway;

            if (payMethod == "upi") {
                std::string upiId = JsonUtil::parseString(body, "upiId");
                std::string pin = JsonUtil::parseString(body, "upiPin");
                gateway = std::unique_ptr<Payment>(new UPIPayment(upiId, pin));
            } else if (payMethod == "card") {
                std::string holder = JsonUtil::parseString(body, "cardHolder");
                std::string number = JsonUtil::parseString(body, "cardNumber");
                std::string expiry = JsonUtil::parseString(body, "expiry");
                std::string cvv = JsonUtil::parseString(body, "cvv");
                gateway = std::unique_ptr<Payment>(new CardPayment(holder, number, expiry, cvv));
            } else if (payMethod == "cod") {
                std::string addr = JsonUtil::parseString(body, "address");
                std::string phone = JsonUtil::parseString(body, "phone");
                gateway = std::unique_ptr<Payment>(new CashOnDeliveryPayment(addr, phone));
            } else {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"success\":false,\"error\":\"Unsupported payment gateway\"}");
                closesocket(clientSocket);
                return;
            }

            double grandTotal = cart.calculateGrandTotal();
            bool paymentOk = gateway->processPayment(grandTotal);

            if (!paymentOk) {
                sendHttpResponse(clientSocket, 400, "Bad Request", "application/json", "{\"success\":false,\"error\":\"Payment gateway verification failed\"}");
                closesocket(clientSocket);
                return;
            }

            for (const auto& item : cart.getItems()) {
                Product* p = catalog.findProductById(item.getProduct().getId());
                if (p) p->reduceStock(item.getQuantity());
            }

            totalRevenue += grandTotal;

            std::ostringstream ss;
            ss << "{"
               << "\"success\":true,"
               << "\"receipt\":{"
               << "\"transactionId\":\"" << gateway->getTransactionId() << "\","
               << "\"timestamp\":\"" << gateway->getTimestamp() << "\","
               << "\"paymentMethod\":\"" << gateway->getMethodName() << "\","
               << "\"details\":\"" << JsonUtil::escape(gateway->getDetails()) << "\","
               << "\"totalUnits\":" << cart.getTotalUnits() << ","
               << "\"grandTotal\":" << std::fixed << std::setprecision(2) << grandTotal << ","
               << "\"items\":[";

            const auto& items = cart.getItems();
            for (size_t i = 0; i < items.size(); ++i) {
                const auto& it = items[i];
                ss << "{"
                   << "\"id\":" << it.getProduct().getId() << ","
                   << "\"name\":\"" << JsonUtil::escape(it.getProduct().getName()) << "\","
                   << "\"price\":" << std::fixed << std::setprecision(2) << it.getProduct().getPrice() << ","
                   << "\"quantity\":" << it.getQuantity() << ","
                   << "\"total\":" << std::fixed << std::setprecision(2) << it.getTotalPrice()
                   << "}";
                if (i + 1 < items.size()) ss << ",";
            }
            ss << "]}}";

            cart.clear();
            sendHttpResponse(clientSocket, 200, "OK", "application/json", ss.str());
        }
        // Static Files
        else {
            std::string relativePath = path;
            if (relativePath == "/" || relativePath.empty()) {
                relativePath = "/index.html";
            }

            std::string fullPath = "public" + relativePath;
            std::string fileData = readFileContent(fullPath);

            if (fileData.empty()) {
                sendHttpResponse(clientSocket, 404, "Not Found", "text/plain", "404 Not Found");
            } else {
                sendHttpResponse(clientSocket, 200, "OK", getMimeType(relativePath), fileData);
            }
        }

        closesocket(clientSocket);
    }

public:
    LocalhostHttpServer(int port = 8080) 
        : port(port), listenSocket(INVALID_SOCKET), totalRevenue(0.0) {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    ~LocalhostHttpServer() {
        if (listenSocket != INVALID_SOCKET) closesocket(listenSocket);
        WSACleanup();
    }

    bool start() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock." << std::endl;
            return false;
        }

        listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (listenSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket." << std::endl;
            WSACleanup();
            return false;
        }

        int opt = 1;
        setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(static_cast<u_short>(port));

        if (bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
            closesocket(listenSocket);
            WSACleanup();
            return false;
        }

        std::cout << "\n======================================================================\n";
        std::cout << "  TECHHOUSE E-COMMERCE & ADMIN PORTAL - LOCALHOST HTTP SERVER (INR Edition)\n";
        std::cout << "  Status: RUNNING\n";
        std::cout << "  Access in Browser at: http://localhost:" << port << "\n";
        std::cout << "  Press Ctrl+C to terminate the server.\n";
        std::cout << "======================================================================\n\n";

        while (true) {
            sockaddr_in clientAddr;
            int clientAddrLen = sizeof(clientAddr);
            SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);

            if (clientSocket != INVALID_SOCKET) {
                handleClient(clientSocket);
            }
        }

        return true;
    }
};

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) port = 8080;
    }

    LocalhostHttpServer server(port);
    server.start();
    return 0;
}
