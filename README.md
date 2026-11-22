# TechHouse — Hardware & Peripherals Store (Console & Web Edition)

A console & localhost web e-commerce platform built in C++ featuring **dual interfaces** (Customer Storefront & Admin Portal), user authentication & registration, dynamic inventory management, and an Amazon-style storefront design with **strictly sharp corners** and a **sand & charcoal neutral palette**.

---

## 1. Dual Interfaces & Key Features

### 1.1 Customer Storefront View
- **Amazon-Style Featured Hero Banner**: Hero deal cards highlighting creator peripherals, workspace gear, key deals, and quick category shortcuts.
- **Product Catalog with Imagery**: Visual product cards with category tags, real-time stock status (*"In stock"*, *"Only X units left"*, *"Out of Stock"*), and "+ Add to Cart".
- **Global Search & Filter**: Real-time search by keywords and instant category filtering (Electronics, Audio, Accessories, Storage, Furniture).
- **Slide-over Shopping Cart Drawer**: Instant quantity adjustments (`+` / `-`), line-item subtotaling, clear cart, and empty states.
- **Polymorphic Checkout**: Choose between UPI, Credit/Debit Card (with live interactive card preview), or Cash on Delivery.
- **Tax Invoice & Order Receipt**: Formatted popup with unique transaction reference, timestamp, line items, and print button.

### 1.2 Admin Management Portal
- **Dashboard Overview Metrics**:
  - Total SKUs in Catalog
  - Total Physical Stock Units in Warehouse
  - Low Stock Alerts (&le; 5 units)
  - Accumulated Store Revenue
- **Product Inventory Directory**:
  - Live table displaying ID, Thumbnail graphic, Product Name, Category, Price, and Stock.
- **Add New Product**: Add new hardware items to the live catalog (Title, Category, Price, Initial Stock, Graphic Type).
- **Manage Prices & Stock**: Inline modal to update unit prices and adjust inventory counts in real time.
- **Remove Product**: Delete obsolete products from the catalog.

### 1.3 User Authentication & Registration
- **Customer Accounts**: Register and sign in to shop.
- **Administrator Accounts**: Protected access to the Admin Management Portal.
- Pre-configured demo credentials:
  - **Admin**: `admin` / `admin123`
  - **Customer**: `user` / `pass123`

---

## 2. Visual & Terminal UI Design

### 2.1 Strict 3-Color ANSI Palette
The terminal interface uses strictly **no more than 3 ANSI colors**:
- **WHITE** (`\033[97m` / `\033[0m`): Body text, descriptions, table labels, and user prompts.
- **ORANGE** (`\033[38;5;208m`): UI headers, frames, borders, dividers, and menu options.
- **GREEN** (`\033[92m`): Product prices, grand totals, order totals, and success confirmations.

### 2.2 Perfect Fixed-Width Typography
All tables and receipts are generated using `<iomanip>` (`std::setw`, `std::left`, `std::right`, `std::fixed`, `std::setprecision(2)`). Columns align uniformly without jagged edges:

```
+------+------------------------------------+---------------+------------+-------+
|  ID  | Product Name                       | Category      | Price      | Stock |
+------+------------------------------------+---------------+------------+-------+
| 101  | Logitech MX Master 3S Mouse        | Electronics   |     $99.99 |    15 |
| 102  | Keychron Q1 Pro Mechanical Keyboard | Electronics   |    $199.50 |     8 |
+------+------------------------------------+---------------+------------+-------+
```

---

## 3. Core Features

1. **Browse Products**: Displays product ID, name, category, unit price, and real-time inventory count.
2. **Add to Cart**: Adds items with requested quantity, ensuring user cannot add more than remaining stock.
3. **Cart Summary**: Lists items, unit prices, selected quantities, line subtotals, and total cost.
4. **Modify / Remove Items**: Adjust quantities on the fly or set quantity to 0 to remove an item.
5. **Clear Cart**: Empties the cart with a confirmation prompt.
6. **Checkout & Order Placement**:
   - Polymorphic payment selection (UPI, Card, COD).
   - Validates user input and simulates authorization.
   - Automatically deducts purchased items from inventory stock.
   - Emits a formatted Tax Invoice / Receipt with transaction ID, timestamp, itemization, and grand total.
7. **Stream Error Resilience**: Safe input helpers prevent non-numeric characters from breaking the menu loop.

---

## 4. Localhost Web Server Architecture (`server.cpp`)

In addition to the console interface, the application includes a **native C++ HTTP Web Server** ([server.cpp](file:///d:/oop%20project/server.cpp)) utilizing Windows WinSock2 (`ws2_32`). It directly interfaces with the exact same OOP classes (`Product`, `Cart`, `CartItem`, polymorphic `Payment` gateways) and serves a modern, responsive web application at `http://localhost:8080`.

### REST Endpoints:
- `GET /` - Serves modern HTML5 web client (`public/index.html`)
- `GET /api/products` - Returns real-time JSON catalog from `Catalog`
- `GET /api/cart` - Returns current `Cart` state, units, and grand total
- `POST /api/cart/add` - Adds item to cart with stock validation (`{"productId": 101, "quantity": 1}`)
- `POST /api/cart/update` - Modifies cart quantity or removes item
- `POST /api/cart/remove` - Removes specific item from cart
- `POST /api/cart/clear` - Clears entire shopping cart
- `POST /api/checkout` - Dynamically instantiates derived `Payment` classes (`UPIPayment`, `CardPayment`, `CashOnDeliveryPayment`), executes `processPayment()`, deducts inventory stock, generates a receipt, and empties the cart.

---

## 5. Compilation & Execution

### 1. Console Application
```powershell
g++ -std=c++11 -Wall -Wextra main.cpp -o shopping_system.exe
.\shopping_system.exe
```

### 2. Localhost Web Server (`http://localhost:8080`)
```powershell
# Compile the native C++ HTTP web server
g++ -std=c++11 -Wall -Wextra server.cpp -o server.exe -lws2_32

# Run the server
.\server.exe
```
Then open your browser to **`http://localhost:8080`**.

