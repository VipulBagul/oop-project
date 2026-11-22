/**
 * TechHouse India - Frontend Application Controller
 * Inspired by Flipkart & Amazon India UI Patterns
 * Currency: Indian Rupees (₹)
 */

document.addEventListener('DOMContentLoaded', () => {
    // ========================================================================
    // STATE
    // ========================================================================
    let products = [];
    let cart = { items: [], itemCount: 0, totalUnits: 0, grandTotal: 0, totalSavings: 0 };
    let currentCategory = 'all';
    let searchQuery = '';
    let currentSort = 'featured';
    let currentUser = null; // { username, fullName, role }
    let currentView = 'storefront'; // 'storefront' or 'admin'
    let selectedPaymentMethod = 'upi';

    // Graphic mappings for tech products
    const productVisuals = {
        mouse: {
            icon: '🖱️',
            url: 'https://images.unsplash.com/photo-1527864550417-7fd91fc51a46?w=500&auto=format&fit=crop&q=80'
        },
        keyboard: {
            icon: '⌨️',
            url: 'https://images.unsplash.com/photo-1587829741301-dc798b83add3?w=500&auto=format&fit=crop&q=80'
        },
        monitor: {
            icon: '🖥️',
            url: 'https://images.unsplash.com/photo-1527443224154-c4a3942d3acf?w=500&auto=format&fit=crop&q=80'
        },
        headphones: {
            icon: '🎧',
            url: 'https://images.unsplash.com/photo-1505740420928-5e560c06d30e?w=500&auto=format&fit=crop&q=80'
        },
        powerbank: {
            icon: '🔋',
            url: 'https://images.unsplash.com/photo-1609592806596-4d8b671a415a?w=500&auto=format&fit=crop&q=80'
        },
        ssd: {
            icon: '💾',
            url: 'https://images.unsplash.com/photo-1597872200969-2b65d56bd16b?w=500&auto=format&fit=crop&q=80'
        },
        stand: {
            icon: '📐',
            url: 'https://images.unsplash.com/photo-1588872657578-7efd1f1555ed?w=500&auto=format&fit=crop&q=80'
        },
        cable: {
            icon: '🔌',
            url: 'https://images.unsplash.com/photo-1544716278-ca5e3f4abd8c?w=500&auto=format&fit=crop&q=80'
        },
        gadget: {
            icon: '⚙️',
            url: 'https://images.unsplash.com/photo-1550009158-9ebf69173e03?w=500&auto=format&fit=crop&q=80'
        }
    };

    // ========================================================================
    // DOM ELEMENTS
    // ========================================================================
    // Header & Views
    const brandLogo = document.getElementById('brandLogo');
    const viewStorefrontBtn = document.getElementById('viewStorefrontBtn');
    const viewAdminBtn = document.getElementById('viewAdminBtn');
    const storefrontView = document.getElementById('storefrontView');
    const adminView = document.getElementById('adminView');
    const globalSearchInput = document.getElementById('globalSearchInput');
    const searchBtn = document.getElementById('searchBtn');
    const searchCategorySelect = document.getElementById('searchCategorySelect');
    const categoryFilters = document.getElementById('categoryFilters');
    const catalogSort = document.getElementById('catalogSort');

    // Auth
    const openAuthModalBtn = document.getElementById('openAuthModalBtn');
    const loggedUserChip = document.getElementById('loggedUserChip');
    const navUserRole = document.getElementById('navUserRole');
    const navUserName = document.getElementById('navUserName');
    const logoutBtn = document.getElementById('logoutBtn');
    const authModal = document.getElementById('authModal');
    const closeAuthModal = document.getElementById('closeAuthModal');
    const authTabs = document.querySelectorAll('.auth-tab');
    const loginForm = document.getElementById('loginForm');
    const registerForm = document.getElementById('registerForm');
    const loginError = document.getElementById('loginError');
    const registerError = document.getElementById('registerError');
    const autofillCustomerBtn = document.getElementById('autofillCustomerBtn');
    const autofillAdminBtn = document.getElementById('autofillAdminBtn');

    // Cart Elements
    const productsGrid = document.getElementById('productsGrid');
    const cartCountBadge = document.getElementById('cartCountBadge');
    const cartNavTotal = document.getElementById('cartNavTotal');
    const openCartBtn = document.getElementById('openCartBtn');
    const closeCartBtn = document.getElementById('closeCartBtn');
    const cartOverlay = document.getElementById('cartOverlay');
    const cartDrawer = document.getElementById('cartDrawer');
    const cartItemsList = document.getElementById('cartItemsList');
    const drawerItemCount = document.getElementById('drawerItemCount');
    const cartUnitsLabel = document.getElementById('cartUnitsLabel');
    const drawerOriginalSubtotal = document.getElementById('drawerOriginalSubtotal');
    const drawerSavings = document.getElementById('drawerSavings');
    const drawerGrandTotal = document.getElementById('drawerGrandTotal');
    const savingsCalloutBanner = document.getElementById('savingsCalloutBanner');
    const clearCartBtn = document.getElementById('clearCartBtn');
    const checkoutBtn = document.getElementById('checkoutBtn');

    // Admin Elements
    const openAddProductModalBtn = document.getElementById('openAddProductModalBtn');
    const addProductModal = document.getElementById('addProductModal');
    const closeAddProductModal = document.getElementById('closeAddProductModal');
    const cancelAddProductBtn = document.getElementById('cancelAddProductBtn');
    const addProductForm = document.getElementById('addProductForm');
    const addProductError = document.getElementById('addProductError');

    const editProductModal = document.getElementById('editProductModal');
    const closeEditProductModal = document.getElementById('closeEditProductModal');
    const cancelEditProductBtn = document.getElementById('cancelEditProductBtn');
    const editProductForm = document.getElementById('editProductForm');
    const editProductSubtitle = document.getElementById('editProductSubtitle');
    const editProdNameDisplay = document.getElementById('editProdNameDisplay');
    const editProdPrice = document.getElementById('editProdPrice');
    const editProdOriginalPrice = document.getElementById('editProdOriginalPrice');
    const editProdStock = document.getElementById('editProdStock');
    const editProdId = document.getElementById('editProdId');
    const editProductError = document.getElementById('editProductError');

    const adminProductsTbody = document.getElementById('adminProductsTbody');
    const adminTableCountBadge = document.getElementById('adminTableCountBadge');
    const statTotalProducts = document.getElementById('statTotalProducts');
    const statTotalStockUnits = document.getElementById('statTotalStockUnits');
    const statLowStockCount = document.getElementById('statLowStockCount');
    const statTotalRevenue = document.getElementById('statTotalRevenue');

    // Checkout Elements
    const checkoutModal = document.getElementById('checkoutModal');
    const closeCheckoutModal = document.getElementById('closeCheckoutModal');
    const cancelCheckoutBtn = document.getElementById('cancelCheckoutBtn');
    const checkoutAmount = document.getElementById('checkoutAmount');
    const payTabs = document.querySelectorAll('.pay-tab');
    const tabPanes = document.querySelectorAll('.tab-pane');
    const confirmPaymentBtn = document.getElementById('confirmPaymentBtn');
    const checkoutError = document.getElementById('checkoutError');
    const btnPayText = document.getElementById('btnPayText');

    // Card Input Mirroring
    const cardHolderInput = document.getElementById('cardHolderInput');
    const cardNumberInput = document.getElementById('cardNumberInput');
    const cardExpiryInput = document.getElementById('cardExpiryInput');
    const cardDisplayName = document.getElementById('cardDisplayName');
    const cardDisplayNumber = document.getElementById('cardDisplayNumber');
    const cardDisplayExpiry = document.getElementById('cardDisplayExpiry');

    // Receipt Elements
    const receiptModal = document.getElementById('receiptModal');
    const receiptContent = document.getElementById('receiptContent');
    const closeReceiptBtn = document.getElementById('closeReceiptBtn');
    const toastContainer = document.getElementById('toastContainer');

    // ========================================================================
    // FORMATTING HELPERS (INDIAN RUPEE ₹ FORMAT)
    // ========================================================================
    function formatCurrency(amount) {
        const val = parseFloat(amount || 0);
        return '₹' + val.toLocaleString('en-IN', {
            minimumFractionDigits: 0,
            maximumFractionDigits: 0
        });
    }

    function formatCurrencyWithPaise(amount) {
        const val = parseFloat(amount || 0);
        return '₹' + val.toLocaleString('en-IN', {
            minimumFractionDigits: 2,
            maximumFractionDigits: 2
        });
    }

    function showToast(message, icon = '✓') {
        const toast = document.createElement('div');
        toast.className = 'toast';
        toast.innerHTML = `<span style="color:var(--accent-sage);font-weight:bold;">${icon}</span> <span>${message}</span>`;
        toastContainer.appendChild(toast);
        setTimeout(() => {
            toast.style.opacity = '0';
            toast.style.transform = 'translateY(10px)';
            toast.style.transition = 'all 0.25s ease';
            setTimeout(() => toast.remove(), 250);
        }, 3000);
    }

    function getVisualForProduct(p) {
        let key = p.image || '';
        if (!productVisuals[key]) {
            const nameLower = (p.name || '').toLowerCase();
            if (nameLower.includes('mouse')) key = 'mouse';
            else if (nameLower.includes('keyboard')) key = 'keyboard';
            else if (nameLower.includes('monitor') || nameLower.includes('display')) key = 'monitor';
            else if (nameLower.includes('headphone') || nameLower.includes('audio')) key = 'headphones';
            else if (nameLower.includes('power') || nameLower.includes('charger')) key = 'powerbank';
            else if (nameLower.includes('ssd') || nameLower.includes('storage') || nameLower.includes('drive')) key = 'ssd';
            else if (nameLower.includes('stand')) key = 'stand';
            else if (nameLower.includes('cable')) key = 'cable';
            else key = 'gadget';
        }
        return productVisuals[key] || productVisuals.gadget;
    }

    // ========================================================================
    // FLASH SALE TICKING TIMER
    // ========================================================================
    let timerTotalSeconds = 4 * 3600 + 28 * 60 + 50;
    setInterval(() => {
        if (timerTotalSeconds > 0) {
            timerTotalSeconds--;
            const h = Math.floor(timerTotalSeconds / 3600);
            const m = Math.floor((timerTotalSeconds % 3600) / 60);
            const s = timerTotalSeconds % 60;
            const hElem = document.getElementById('timerHours');
            const mElem = document.getElementById('timerMins');
            const sElem = document.getElementById('timerSecs');
            if (hElem) hElem.textContent = String(h).padStart(2, '0');
            if (mElem) mElem.textContent = String(m).padStart(2, '0');
            if (sElem) sElem.textContent = String(s).padStart(2, '0');
        }
    }, 1000);

    // ========================================================================
    // AUTHENTICATION LOGIC
    // ========================================================================
    function loadSavedUser() {
        try {
            const saved = localStorage.getItem('techhouse_user');
            if (saved) {
                currentUser = JSON.parse(saved);
                updateAuthUI();
            }
        } catch (e) {
            console.error('Failed to parse saved user:', e);
        }
    }

    function updateAuthUI() {
        if (currentUser) {
            openAuthModalBtn.style.display = 'none';
            loggedUserChip.style.display = 'inline-flex';
            navUserName.textContent = currentUser.fullName || currentUser.username;
            navUserRole.textContent = currentUser.role === 'admin' ? 'Admin' : 'Customer';
            if (currentUser.role === 'admin') {
                navUserRole.style.background = 'var(--text-charcoal)';
            } else {
                navUserRole.style.background = 'var(--accent-earth)';
            }
        } else {
            openAuthModalBtn.style.display = 'flex';
            loggedUserChip.style.display = 'none';
        }
    }

    function openAuthModal(defaultTab = 'login', customTitle = null) {
        loginError.style.display = 'none';
        registerError.style.display = 'none';
        if (customTitle) {
            document.getElementById('authModalTitle').textContent = customTitle;
        } else {
            document.getElementById('authModalTitle').textContent = 'Account Authentication';
        }

        authTabs.forEach(t => t.classList.toggle('active', t.getAttribute('data-tab') === defaultTab));
        loginForm.classList.toggle('active', defaultTab === 'login');
        registerForm.classList.toggle('active', defaultTab === 'register');

        authModal.classList.add('active');
    }

    function closeAuth() {
        authModal.classList.remove('active');
    }

    openAuthModalBtn.addEventListener('click', () => openAuthModal('login'));
    closeAuthModal.addEventListener('click', closeAuth);

    authTabs.forEach(tab => {
        tab.addEventListener('click', () => {
            const target = tab.getAttribute('data-tab');
            authTabs.forEach(t => t.classList.remove('active'));
            tab.classList.add('active');
            loginForm.classList.toggle('active', target === 'login');
            registerForm.classList.toggle('active', target === 'register');
            loginError.style.display = 'none';
            registerError.style.display = 'none';
        });
    });

    autofillCustomerBtn.addEventListener('click', () => {
        document.getElementById('loginUsername').value = 'user';
        document.getElementById('loginPassword').value = 'pass123';
    });

    autofillAdminBtn.addEventListener('click', () => {
        document.getElementById('loginUsername').value = 'admin';
        document.getElementById('loginPassword').value = 'admin123';
    });

    loginForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        loginError.style.display = 'none';

        const username = document.getElementById('loginUsername').value.trim();
        const password = document.getElementById('loginPassword').value.trim();

        try {
            const res = await fetch('/api/auth/login', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ username, password })
            });
            const data = await res.json();

            if (!data.success) {
                loginError.textContent = data.error || 'Invalid credentials';
                loginError.style.display = 'block';
                return;
            }

            currentUser = data.user;
            localStorage.setItem('techhouse_user', JSON.stringify(currentUser));
            updateAuthUI();
            closeAuth();
            showToast(`Welcome back, ${currentUser.fullName}!`);

            if (currentView === 'admin' && currentUser.role !== 'admin') {
                switchView('storefront');
            } else if (currentView === 'admin') {
                loadAdminData();
            }
        } catch (err) {
            loginError.textContent = 'Server communication error';
            loginError.style.display = 'block';
        }
    });

    registerForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        registerError.style.display = 'none';

        const fullName = document.getElementById('regFullName').value.trim();
        const username = document.getElementById('regUsername').value.trim();
        const password = document.getElementById('regPassword').value.trim();
        const role = document.getElementById('regRole').value;

        try {
            const res = await fetch('/api/auth/register', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ fullName, username, password, role })
            });
            const data = await res.json();

            if (!data.success) {
                registerError.textContent = data.error || 'Registration failed';
                registerError.style.display = 'block';
                return;
            }

            showToast('Account registered! Please sign in.');
            document.querySelector('.auth-tab[data-tab="login"]').click();
            document.getElementById('loginUsername').value = username;
            document.getElementById('loginPassword').value = password;
        } catch (err) {
            registerError.textContent = 'Server communication error';
            registerError.style.display = 'block';
        }
    });

    logoutBtn.addEventListener('click', () => {
        currentUser = null;
        localStorage.removeItem('techhouse_user');
        updateAuthUI();
        showToast('Signed out successfully');
        if (currentView === 'admin') {
            switchView('storefront');
        }
    });

    // ========================================================================
    // VIEW SWITCHING (STOREFRONT VS ADMIN)
    // ========================================================================
    function switchView(target) {
        if (target === 'admin') {
            if (!currentUser || currentUser.role !== 'admin') {
                openAuthModal('login', 'Merchant Admin Sign In Required');
                document.getElementById('loginUsername').value = 'admin';
                document.getElementById('loginPassword').value = 'admin123';
                showToast('Please sign in with your Administrator account', '🔒');
                return;
            }
            currentView = 'admin';
            viewAdminBtn.classList.add('active');
            viewStorefrontBtn.classList.remove('active');
            adminView.classList.add('active');
            storefrontView.classList.remove('active');
            loadAdminData();
        } else {
            currentView = 'storefront';
            viewStorefrontBtn.classList.add('active');
            viewAdminBtn.classList.remove('active');
            storefrontView.classList.add('active');
            adminView.classList.remove('active');
            renderProducts();
        }
    }

    viewStorefrontBtn.addEventListener('click', () => switchView('storefront'));
    viewAdminBtn.addEventListener('click', () => switchView('admin'));
    brandLogo.addEventListener('click', () => switchView('storefront'));

    // ========================================================================
    // STOREFRONT LOGIC (INDIAN RUPEE PRICING & FLIPKART/AMAZON CARDS)
    // ========================================================================
    async function loadProducts() {
        try {
            const res = await fetch('/api/products');
            if (!res.ok) throw new Error('Network error');
            products = await res.json();
            renderProducts();
            if (currentView === 'admin') renderAdminTable();
        } catch (err) {
            console.error('Failed to load products:', err);
            productsGrid.innerHTML = `
                <div class="empty-cart-state" style="grid-column: 1 / -1;">
                    <div class="empty-cart-icon">⚠️</div>
                    <h4>Server Connection Failed</h4>
                    <p>Make sure the backend executable is running on port 8080.</p>
                </div>
            `;
        }
    }

    async function loadCart() {
        try {
            const res = await fetch('/api/cart');
            if (!res.ok) throw new Error('Cart fetch error');
            cart = await res.json();
            updateCartUI();
        } catch (err) {
            console.error('Failed to load cart:', err);
        }
    }

    function renderProducts() {
        let filtered = products.slice();

        // Category filter
        if (currentCategory !== 'all') {
            filtered = filtered.filter(p => p.category.toLowerCase() === currentCategory.toLowerCase());
        }

        // Search query filter
        if (searchQuery) {
            const q = searchQuery.toLowerCase();
            filtered = filtered.filter(p => 
                p.name.toLowerCase().includes(q) || 
                p.category.toLowerCase().includes(q)
            );
        }

        // Sorting
        if (currentSort === 'price-low') {
            filtered.sort((a, b) => a.price - b.price);
        } else if (currentSort === 'price-high') {
            filtered.sort((a, b) => b.price - a.price);
        } else if (currentSort === 'rating') {
            filtered.sort((a, b) => (b.rating || 0) - (a.rating || 0));
        }

        if (filtered.length === 0) {
            productsGrid.innerHTML = `
                <div class="empty-cart-state" style="grid-column: 1 / -1;">
                    <div class="empty-cart-icon">🔍</div>
                    <h4>No Matching Products Found</h4>
                    <p>Try searching for mechanical keyboards, monitors, or clear filters.</p>
                </div>
            `;
            return;
        }

        productsGrid.innerHTML = filtered.map(p => {
            let stockClass = 'in-stock';
            let stockText = `${p.stock} in stock`;
            if (p.stock <= 0) {
                stockClass = 'out-of-stock';
                stockText = 'Out of Stock';
            } else if (p.stock <= 5) {
                stockClass = 'low-stock';
                stockText = `Only ${p.stock} units left`;
            }

            const cartItem = cart.items ? cart.items.find(item => item.id === p.id) : null;
            const inCartQty = cartItem ? cartItem.quantity : 0;
            const isMaxed = inCartQty >= p.stock;
            const visual = getVisualForProduct(p);

            // Calculate discount percentage like Flipkart/Amazon
            const orig = p.originalPrice > p.price ? p.originalPrice : p.price * 1.35;
            const discountPct = Math.round(((orig - p.price) / orig) * 100);

            const rating = (p.rating || 4.6).toFixed(1);
            const reviews = (p.reviewCount || 1240).toLocaleString('en-IN');

            return `
                <article class="product-card" data-id="${p.id}">
                    <div class="product-card-visual">
                        <img src="${visual.url}" alt="${p.name}" class="product-card-img" onerror="this.style.display='none'; this.nextElementSibling.style.display='block';">
                        <div class="product-graphic-icon" style="display:none;">${visual.icon}</div>
                        <span class="card-assured-badge">Assured ★</span>
                        <span class="card-sku-overlay">#${p.id}</span>
                    </div>

                    <div class="product-card-body">
                        <h4 class="product-title">${p.name}</h4>

                        <!-- Flipkart / Amazon Rating Row -->
                        <div class="product-rating-row">
                            <span class="rating-pill">★ ${rating}</span>
                            <span class="review-count-text">(${reviews})</span>
                        </div>

                        <!-- Flipkart Pricing Block -->
                        <div class="product-pricing-block">
                            <span class="selling-price">${formatCurrency(p.price)}</span>
                            <span class="original-price-mrp">${formatCurrency(orig)}</span>
                            <span class="discount-tag">${discountPct}% off</span>
                        </div>

                        <div class="delivery-badge-line">⚡ FREE Delivery by Tomorrow</div>
                        <div class="stock-indicator ${stockClass}">${stockText}</div>
                        
                        <div class="card-action-bar">
                            <button class="add-cart-btn" data-id="${p.id}" ${p.stock <= 0 || isMaxed ? 'disabled' : ''}>
                                <span>${p.stock <= 0 ? 'Sold Out' : isMaxed ? 'Max in Cart' : '+ Add to Cart'}</span>
                            </button>
                        </div>
                    </div>
                </article>
            `;
        }).join('');

        // Attach listeners
        document.querySelectorAll('.add-cart-btn').forEach(btn => {
            btn.addEventListener('click', (e) => {
                const prodId = parseInt(e.currentTarget.getAttribute('data-id'));
                addToCart(prodId, 1);
            });
        });
    }

    async function addToCart(productId, quantity) {
        try {
            const res = await fetch('/api/cart/add', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ productId, quantity })
            });
            const data = await res.json();
            if (data.error) {
                showToast(data.error, '⚠️');
                return;
            }
            cart = data;
            updateCartUI();
            const prod = products.find(p => p.id === productId);
            showToast(`Added ${prod ? prod.name : 'Item'} to cart!`);
            renderProducts();
        } catch (err) {
            console.error('Add to cart error:', err);
        }
    }

    async function updateCartItem(productId, newQuantity) {
        try {
            const res = await fetch('/api/cart/update', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ productId, quantity: newQuantity })
            });
            const data = await res.json();
            if (data.error) {
                showToast(data.error, '⚠️');
                return;
            }
            cart = data;
            updateCartUI();
            renderProducts();
        } catch (err) {
            console.error('Update cart error:', err);
        }
    }

    async function removeCartItem(productId) {
        try {
            const res = await fetch('/api/cart/remove', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ productId })
            });
            cart = await res.json();
            updateCartUI();
            renderProducts();
            showToast('Item removed from cart');
        } catch (err) {
            console.error('Remove item error:', err);
        }
    }

    async function clearCart() {
        if (!cart.items || cart.items.length === 0) return;
        if (!confirm('Are you sure you want to clear your shopping cart?')) return;

        try {
            const res = await fetch('/api/cart/clear', { method: 'POST' });
            cart = await res.json();
            updateCartUI();
            renderProducts();
            showToast('Cart cleared');
        } catch (err) {
            console.error('Clear cart error:', err);
        }
    }

    function updateCartUI() {
        const totalUnits = cart.totalUnits || 0;
        const grandTotal = cart.grandTotal || 0;
        const totalSavings = cart.totalSavings || 0;
        const totalOriginal = grandTotal + totalSavings;

        cartCountBadge.textContent = totalUnits;
        cartNavTotal.textContent = formatCurrency(grandTotal);
        drawerItemCount.textContent = `${totalUnits} item${totalUnits === 1 ? '' : 's'}`;
        cartUnitsLabel.textContent = `${totalUnits} item${totalUnits === 1 ? '' : 's'}`;
        drawerOriginalSubtotal.textContent = formatCurrency(totalOriginal);
        drawerSavings.textContent = `- ${formatCurrency(totalSavings)}`;
        drawerGrandTotal.textContent = formatCurrency(grandTotal);

        if (totalSavings > 0) {
            savingsCalloutBanner.style.display = 'block';
            savingsCalloutBanner.textContent = `🎉 You will save ${formatCurrency(totalSavings)} on this order!`;
        } else {
            savingsCalloutBanner.style.display = 'none';
        }

        if (!cart.items || cart.items.length === 0) {
            cartItemsList.innerHTML = `
                <div class="empty-cart-state">
                    <div class="empty-cart-icon">🛒</div>
                    <h4>Your Cart is Empty</h4>
                    <p>Explore today's deals and add items to your cart.</p>
                </div>
            `;
            checkoutBtn.disabled = true;
            clearCartBtn.disabled = true;
        } else {
            checkoutBtn.disabled = false;
            clearCartBtn.disabled = false;
            cartItemsList.innerHTML = cart.items.map(item => {
                const prod = products.find(p => p.id === item.id);
                const maxStock = prod ? prod.stock : 99;

                return `
                    <div class="cart-item-row" data-id="${item.id}">
                        <div class="cart-item-header">
                            <span class="cart-item-name">${item.name}</span>
                            <button class="cart-item-remove-btn" data-id="${item.id}" title="Remove item">&times;</button>
                        </div>
                        <div class="cart-item-footer">
                            <div class="qty-control">
                                <button class="qty-btn btn-minus" data-id="${item.id}" data-qty="${item.quantity - 1}">-</button>
                                <input type="text" class="qty-input" value="${item.quantity}" readonly>
                                <button class="qty-btn btn-plus" data-id="${item.id}" data-qty="${item.quantity + 1}" ${item.quantity >= maxStock ? 'disabled' : ''}>+</button>
                            </div>
                            <div class="cart-item-pricing">
                                <div class="cart-item-unit-price">${formatCurrency(item.price)} each</div>
                                <div class="cart-item-subtotal">${formatCurrency(item.total)}</div>
                            </div>
                        </div>
                    </div>
                `;
            }).join('');

            cartItemsList.querySelectorAll('.cart-item-remove-btn').forEach(btn => {
                btn.addEventListener('click', () => {
                    const id = parseInt(btn.getAttribute('data-id'));
                    removeCartItem(id);
                });
            });

            cartItemsList.querySelectorAll('.btn-minus').forEach(btn => {
                btn.addEventListener('click', () => {
                    const id = parseInt(btn.getAttribute('data-id'));
                    const newQty = parseInt(btn.getAttribute('data-qty'));
                    updateCartItem(id, newQty);
                });
            });

            cartItemsList.querySelectorAll('.btn-plus').forEach(btn => {
                btn.addEventListener('click', () => {
                    const id = parseInt(btn.getAttribute('data-id'));
                    const newQty = parseInt(btn.getAttribute('data-qty'));
                    updateCartItem(id, newQty);
                });
            });
        }
    }

    // Category Strip Navigation
    categoryFilters.querySelectorAll('.cat-strip-btn').forEach(btn => {
        btn.addEventListener('click', () => {
            categoryFilters.querySelectorAll('.cat-strip-btn').forEach(b => b.classList.remove('active'));
            btn.classList.add('active');
            currentCategory = btn.getAttribute('data-category');
            renderProducts();
        });
    });

    // Deal Quad Cards Click
    document.querySelectorAll('.deal-quad-card').forEach(card => {
        card.addEventListener('click', () => {
            const cat = card.getAttribute('data-category');
            if (cat) {
                const targetBtn = Array.from(categoryFilters.querySelectorAll('.cat-strip-btn')).find(b => b.getAttribute('data-category').toLowerCase() === cat.toLowerCase());
                if (targetBtn) targetBtn.click();
                document.getElementById('catalogSection').scrollIntoView({ behavior: 'smooth' });
            }
        });
    });

    // Catalog Sort Select
    catalogSort.addEventListener('change', (e) => {
        currentSort = e.target.value;
        renderProducts();
    });

    // Search bar
    globalSearchInput.addEventListener('input', (e) => {
        searchQuery = e.target.value.trim();
        renderProducts();
    });

    searchBtn.addEventListener('click', () => {
        searchQuery = globalSearchInput.value.trim();
        renderProducts();
    });

    searchCategorySelect.addEventListener('change', (e) => {
        currentCategory = e.target.value;
        categoryFilters.querySelectorAll('.cat-strip-btn').forEach(b => {
            b.classList.toggle('active', b.getAttribute('data-category').toLowerCase() === currentCategory.toLowerCase());
        });
        renderProducts();
    });

    // Drawer Controls
    function openDrawer() {
        cartOverlay.classList.add('active');
        cartDrawer.classList.add('active');
        cartDrawer.setAttribute('aria-hidden', 'false');
    }

    function closeDrawer() {
        cartOverlay.classList.remove('active');
        cartDrawer.classList.remove('active');
        cartDrawer.setAttribute('aria-hidden', 'true');
    }

    openCartBtn.addEventListener('click', openDrawer);
    closeCartBtn.addEventListener('click', closeDrawer);
    cartOverlay.addEventListener('click', closeDrawer);
    clearCartBtn.addEventListener('click', clearCart);

    // ========================================================================
    // ADMIN PORTAL LOGIC (IN RUPEES ₹)
    // ========================================================================
    async function loadAdminData() {
        await loadProducts();
        try {
            const res = await fetch('/api/admin/stats');
            if (res.ok) {
                const stats = await res.json();
                statTotalProducts.textContent = stats.totalProducts;
                statTotalStockUnits.textContent = stats.totalStockUnits;
                statLowStockCount.textContent = stats.lowStockCount;
                statTotalRevenue.textContent = formatCurrencyWithPaise(stats.totalRevenue);
            }
        } catch (e) {
            console.error('Failed to load admin stats:', e);
        }
        renderAdminTable();
    }

    function renderAdminTable() {
        adminTableCountBadge.textContent = `${products.length} Items Listed`;
        adminProductsTbody.innerHTML = products.map(p => {
            const visual = getVisualForProduct(p);
            let stockBadge = '<span style="color:var(--accent-sage);font-weight:700;">' + p.stock + ' units</span>';
            if (p.stock <= 0) {
                stockBadge = '<span style="color:#991b1b;font-weight:700;">Out of Stock (0)</span>';
            } else if (p.stock <= 5) {
                stockBadge = '<span style="color:#9e6015;font-weight:700;">Low Stock (' + p.stock + ')</span>';
            }

            const orig = p.originalPrice > 0 ? p.originalPrice : p.price * 1.35;

            return `
                <tr data-id="${p.id}">
                    <td style="font-family:var(--font-mono);font-weight:700;">#${p.id}</td>
                    <td>
                        <div class="admin-product-thumb">${visual.icon}</div>
                    </td>
                    <td>
                        <strong style="font-size:0.95rem;color:var(--text-charcoal);">${p.name}</strong>
                    </td>
                    <td>
                        <span style="background:var(--bg-sand);border:1px solid var(--border-sand);padding:0.2rem 0.5rem;font-size:0.75rem;font-weight:700;">${p.category}</span>
                    </td>
                    <td style="text-align:right;font-family:var(--font-mono);font-weight:700;color:var(--accent-sage);">
                        ${formatCurrency(p.price)}
                    </td>
                    <td style="text-align:right;font-family:var(--font-mono);font-weight:600;color:var(--text-charcoal-muted);text-decoration:line-through;">
                        ${formatCurrency(orig)}
                    </td>
                    <td style="text-align:center;">
                        ${stockBadge}
                    </td>
                    <td>
                        <div class="admin-actions-cell">
                            <button class="admin-btn-action btn-edit" data-id="${p.id}" title="Edit price & stock">Edit</button>
                            <button class="admin-btn-action btn-delete" data-id="${p.id}" title="Delete product">Delete</button>
                        </div>
                    </td>
                </tr>
            `;
        }).join('');

        adminProductsTbody.querySelectorAll('.btn-edit').forEach(btn => {
            btn.addEventListener('click', () => {
                const id = parseInt(btn.getAttribute('data-id'));
                openEditProductModal(id);
            });
        });

        adminProductsTbody.querySelectorAll('.btn-delete').forEach(btn => {
            btn.addEventListener('click', () => {
                const id = parseInt(btn.getAttribute('data-id'));
                handleDeleteProduct(id);
            });
        });
    }

    // Add Product Modal
    openAddProductModalBtn.addEventListener('click', () => {
        addProductForm.reset();
        addProductError.style.display = 'none';
        addProductModal.classList.add('active');
    });

    function closeAddProduct() {
        addProductModal.classList.remove('active');
    }

    closeAddProductModal.addEventListener('click', closeAddProduct);
    cancelAddProductBtn.addEventListener('click', closeAddProduct);

    addProductForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        addProductError.style.display = 'none';

        const name = document.getElementById('newProdName').value.trim();
        const category = document.getElementById('newProdCategory').value;
        const price = parseFloat(document.getElementById('newProdPrice').value);
        const originalPrice = parseFloat(document.getElementById('newProdOriginalPrice').value) || (price * 1.35);
        const stock = parseInt(document.getElementById('newProdStock').value);
        const image = document.getElementById('newProdImage').value;

        if (!name || isNaN(price) || isNaN(stock)) {
            addProductError.textContent = 'Please provide valid product details.';
            addProductError.style.display = 'block';
            return;
        }

        try {
            const res = await fetch('/api/admin/products/add', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ name, category, price, originalPrice, stock, image })
            });

            if (!res.ok) {
                const err = await res.json();
                addProductError.textContent = err.error || 'Failed to add product';
                addProductError.style.display = 'block';
                return;
            }

            products = await res.json();
            closeAddProduct();
            showToast(`Added '${name}' with price ${formatCurrency(price)}!`);
            loadAdminData();
        } catch (err) {
            addProductError.textContent = 'Server communication error';
            addProductError.style.display = 'block';
        }
    });

    // Edit Product Modal
    function openEditProductModal(id) {
        const prod = products.find(p => p.id === id);
        if (!prod) return;

        editProdId.value = prod.id;
        editProdNameDisplay.value = prod.name;
        editProdPrice.value = prod.price;
        editProdOriginalPrice.value = prod.originalPrice || Math.round(prod.price * 1.35);
        editProdStock.value = prod.stock;
        editProductSubtitle.textContent = `Adjusting SKU #${prod.id} (${prod.category})`;
        editProductError.style.display = 'none';

        editProductModal.classList.add('active');
    }

    function closeEditProduct() {
        editProductModal.classList.remove('active');
    }

    closeEditProductModal.addEventListener('click', closeEditProduct);
    cancelEditProductBtn.addEventListener('click', closeEditProduct);

    editProductForm.addEventListener('submit', async (e) => {
        e.preventDefault();
        editProductError.style.display = 'none';

        const id = parseInt(editProdId.value);
        const price = parseFloat(editProdPrice.value);
        const originalPrice = parseFloat(editProdOriginalPrice.value) || (price * 1.35);
        const stock = parseInt(editProdStock.value);

        if (isNaN(price) || isNaN(stock)) {
            editProductError.textContent = 'Invalid price or stock quantity.';
            editProductError.style.display = 'block';
            return;
        }

        try {
            const res = await fetch('/api/admin/products/update', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id, price, originalPrice, stock })
            });

            if (!res.ok) {
                const err = await res.json();
                editProductError.textContent = err.error || 'Failed to update';
                editProductError.style.display = 'block';
                return;
            }

            products = await res.json();
            closeEditProduct();
            showToast(`Updated product details to ${formatCurrency(price)}!`);
            loadAdminData();
        } catch (err) {
            editProductError.textContent = 'Server communication error';
            editProductError.style.display = 'block';
        }
    });

    async function handleDeleteProduct(id) {
        const prod = products.find(p => p.id === id);
        const name = prod ? prod.name : `Product #${id}`;
        if (!confirm(`Are you sure you want to permanently delete '${name}' from inventory?`)) {
            return;
        }

        try {
            const res = await fetch('/api/admin/products/delete', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ id })
            });

            if (!res.ok) {
                showToast('Failed to delete product', '⚠️');
                return;
            }

            products = await res.json();
            showToast(`Deleted '${name}' from catalog.`);
            loadAdminData();
            loadCart();
        } catch (err) {
            console.error('Delete product error:', err);
        }
    }

    // ========================================================================
    // CHECKOUT & RECEIPT LOGIC (RUPEES ₹)
    // ========================================================================
    checkoutBtn.addEventListener('click', () => {
        if (!cart.items || cart.items.length === 0) return;
        closeDrawer();
        checkoutAmount.textContent = formatCurrency(cart.grandTotal);
        checkoutError.style.display = 'none';
        checkoutModal.classList.add('active');
    });

    function closeCheckout() {
        checkoutModal.classList.remove('active');
    }

    closeCheckoutModal.addEventListener('click', closeCheckout);
    cancelCheckoutBtn.addEventListener('click', closeCheckout);

    payTabs.forEach(tab => {
        tab.addEventListener('click', () => {
            payTabs.forEach(t => t.classList.remove('active'));
            tabPanes.forEach(p => p.classList.remove('active'));

            tab.classList.add('active');
            selectedPaymentMethod = tab.getAttribute('data-method');
            document.getElementById(`pane-${selectedPaymentMethod}`).classList.add('active');
            checkoutError.style.display = 'none';
        });
    });

    cardNumberInput.addEventListener('input', (e) => {
        let val = e.target.value.replace(/\D/g, '').substring(0, 16);
        let formatted = val.match(/.{1,4}/g)?.join(' ') || '';
        e.target.value = formatted;
        cardDisplayNumber.textContent = formatted || '•••• •••• •••• ••••';
    });

    cardHolderInput.addEventListener('input', (e) => {
        cardDisplayName.textContent = (e.target.value || 'AARAV SHARMA').toUpperCase();
    });

    cardExpiryInput.addEventListener('input', (e) => {
        let val = e.target.value.replace(/\D/g, '').substring(0, 4);
        if (val.length >= 3) {
            val = val.substring(0, 2) + '/' + val.substring(2);
        }
        e.target.value = val;
        cardDisplayExpiry.textContent = val || 'MM/YY';
    });

    confirmPaymentBtn.addEventListener('click', async () => {
        checkoutError.style.display = 'none';
        let payload = { paymentMethod: selectedPaymentMethod };

        if (selectedPaymentMethod === 'upi') {
            const upiId = document.getElementById('upiIdInput').value.trim();
            const pin = document.getElementById('upiPinInput').value.trim();
            if (!upiId.includes('@') || upiId.length < 4) {
                showCheckoutError('Please enter a valid UPI ID (e.g. user@okhdfcbank / user@paytm)');
                return;
            }
            if (pin.length < 4 || pin.length > 6) {
                showCheckoutError('UPI Security PIN must be 4 or 6 digits');
                return;
            }
            payload.upiId = upiId;
            payload.upiPin = pin;
        } else if (selectedPaymentMethod === 'card') {
            const name = cardHolderInput.value.trim();
            const rawNumber = cardNumberInput.value.replace(/\s+/g, '');
            const expiry = cardExpiryInput.value.trim();
            const cvv = document.getElementById('cardCvvInput').value.trim();

            if (!name) {
                showCheckoutError('Cardholder Name is required');
                return;
            }
            if (rawNumber.length !== 16) {
                showCheckoutError('Card Number must be exactly 16 digits');
                return;
            }
            if (expiry.length !== 5) {
                showCheckoutError('Card Expiry must be in MM/YY format');
                return;
            }
            if (cvv.length !== 3) {
                showCheckoutError('CVV must be 3 digits');
                return;
            }
            payload.cardHolder = name;
            payload.cardNumber = rawNumber;
            payload.expiry = expiry;
            payload.cvv = cvv;
        } else if (selectedPaymentMethod === 'cod') {
            const address = document.getElementById('codAddressInput').value.trim();
            const phone = document.getElementById('codPhoneInput').value.trim();
            if (address.length < 10) {
                showCheckoutError('Please enter a complete delivery address with pincode');
                return;
            }
            if (phone.length < 8) {
                showCheckoutError('Please enter a valid contact phone number');
                return;
            }
            payload.address = address;
            payload.phone = phone;
        }

        btnPayText.textContent = 'Processing Payment...';
        confirmPaymentBtn.disabled = true;

        try {
            const res = await fetch('/api/checkout', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(payload)
            });
            const data = await res.json();

            btnPayText.textContent = 'Authorize & Pay';
            confirmPaymentBtn.disabled = false;

            if (!data.success) {
                showCheckoutError(data.error || 'Payment transaction declined');
                return;
            }

            closeCheckout();
            showReceipt(data.receipt);
            await loadProducts();
            await loadCart();
        } catch (err) {
            btnPayText.textContent = 'Authorize & Pay';
            confirmPaymentBtn.disabled = false;
            showCheckoutError('Server communication failed during checkout');
            console.error(err);
        }
    });

    function showCheckoutError(msg) {
        checkoutError.textContent = msg;
        checkoutError.style.display = 'block';
    }

    function showReceipt(receipt) {
        receiptContent.innerHTML = `
            <div class="receipt-meta-box">
                <div class="receipt-meta-item">
                    <span class="meta-label">Transaction Ref:</span>
                    <span class="meta-val">${receipt.transactionId}</span>
                </div>
                <div class="receipt-meta-item">
                    <span class="meta-label">Date & Time:</span>
                    <span class="meta-val">${receipt.timestamp}</span>
                </div>
                <div class="receipt-meta-item">
                    <span class="meta-label">Payment Method:</span>
                    <span class="meta-val">${receipt.paymentMethod}</span>
                </div>
                <div class="receipt-meta-item">
                    <span class="meta-label">Account Details:</span>
                    <span class="meta-val">${receipt.details}</span>
                </div>
            </div>

            <table class="receipt-table">
                <thead>
                    <tr>
                        <th>Item Description</th>
                        <th>Unit Price</th>
                        <th style="text-align:center;">Qty</th>
                        <th style="text-align:right;">Subtotal</th>
                    </tr>
                </thead>
                <tbody>
                    ${receipt.items.map(it => `
                        <tr>
                            <td><strong>${it.name}</strong></td>
                            <td>${formatCurrency(it.price)}</td>
                            <td style="text-align:center;">${it.quantity}</td>
                            <td style="text-align:right;" class="receipt-total-col">${formatCurrency(it.total)}</td>
                        </tr>
                    `).join('')}
                    <tr class="receipt-grand-total-row">
                        <td colspan="2">TOTAL AMOUNT BILLED</td>
                        <td style="text-align:center;">${receipt.totalUnits} items</td>
                        <td style="text-align:right;" class="receipt-total-col">${formatCurrency(receipt.grandTotal)}</td>
                    </tr>
                </tbody>
            </table>
        `;
        receiptModal.classList.add('active');
    }

    closeReceiptBtn.addEventListener('click', () => {
        receiptModal.classList.remove('active');
    });

    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    loadSavedUser();
    loadProducts();
    loadCart();
});
