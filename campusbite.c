#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 50
#define MAX_CART_ITEMS 20
#define MAX_ORDERS 100

typedef struct {
    int id;
    char name[40];
    float price;
    int stock;
} MenuItem;

typedef struct {
    int itemId;
    char name[40];
    float price;
    int quantity;
} OrderItem;

typedef struct {
    int orderId;
    char studentName[50];
    OrderItem items[MAX_CART_ITEMS];
    int itemCount;
    float total;
    char paymentMode[10];
    char status[20];
} Order;

MenuItem menu[MAX_ITEMS];
Order orders[MAX_ORDERS];
int menuCount = 0;
int orderCount = 0;

void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void readLine(char text[], int size) {
    if (fgets(text, size, stdin) != NULL) {
        text[strcspn(text, "\n")] = '\0';
    }
}

int findMenuItem(int id) {
    int i;
    for (i = 0; i < menuCount; i++) {
        if (menu[i].id == id) {
            return i;
        }
    }
    return -1;
}

int findOrder(int orderId) {
    int i;
    for (i = 0; i < orderCount; i++) {
        if (orders[i].orderId == orderId) {
            return i;
        }
    }
    return -1;
}

void saveData(void) {
    FILE *file;

    file = fopen("menu.dat", "wb");
    if (file != NULL) {
        fwrite(&menuCount, sizeof(int), 1, file);
        fwrite(menu, sizeof(MenuItem), menuCount, file);
        fclose(file);
    }

    file = fopen("orders.dat", "wb");
    if (file != NULL) {
        fwrite(&orderCount, sizeof(int), 1, file);
        fwrite(orders, sizeof(Order), orderCount, file);
        fclose(file);
    }
}

void loadData(void) {
    FILE *file;

    file = fopen("menu.dat", "rb");
    if (file != NULL) {
        fread(&menuCount, sizeof(int), 1, file);
        if (menuCount < 0 || menuCount > MAX_ITEMS) {
            menuCount = 0;
        } else {
            fread(menu, sizeof(MenuItem), menuCount, file);
        }
        fclose(file);
    }

    file = fopen("orders.dat", "rb");
    if (file != NULL) {
        fread(&orderCount, sizeof(int), 1, file);
        if (orderCount < 0 || orderCount > MAX_ORDERS) {
            orderCount = 0;
        } else {
            fread(orders, sizeof(Order), orderCount, file);
        }
        fclose(file);
    }
}

void addDefaultMenu(void) {
    if (menuCount != 0) {
        return;
    }

    menu[0] = (MenuItem){101, "Samosa", 15.0f, 30};
    menu[1] = (MenuItem){102, "Veg Burger", 50.0f, 20};
    menu[2] = (MenuItem){103, "Sandwich", 40.0f, 20};
    menu[3] = (MenuItem){104, "Cold Coffee", 45.0f, 15};
    menu[4] = (MenuItem){105, "Tea", 12.0f, 40};
    menuCount = 5;
    saveData();
}

void displayMenu(void) {
    int i;
    printf("\n---------------- CANTEEN MENU ----------------\n");
    printf("%-6s %-22s %-10s %-10s\n", "ID", "Item", "Price", "Stock");
    printf("----------------------------------------------\n");
    for (i = 0; i < menuCount; i++) {
        printf("%-6d %-22s Rs.%-7.2f %-10d\n",
               menu[i].id, menu[i].name, menu[i].price, menu[i].stock);
    }
}

void printBill(const Order *order) {
    int i;
    printf("\n================ CAMPUSBITE BILL ================\n");
    printf("Order ID : %d\n", order->orderId);
    printf("Student  : %s\n", order->studentName);
    printf("------------------------------------------------\n");
    printf("%-20s %-6s %-8s %-10s\n", "Item", "Qty", "Price", "Amount");
    for (i = 0; i < order->itemCount; i++) {
        printf("%-20s %-6d %-8.2f %-10.2f\n",
               order->items[i].name,
               order->items[i].quantity,
               order->items[i].price,
               order->items[i].price * order->items[i].quantity);
    }
    printf("------------------------------------------------\n");
    printf("Total        : Rs. %.2f\n", order->total);
    printf("Payment mode : %s\n", order->paymentMode);
    printf("Status       : %s\n", order->status);
    printf("================================================\n");
}

void placeOrder(void) {
    Order newOrder = {0};
    int id, quantity, index, choice, i;
    int cartIndex;

    if (orderCount >= MAX_ORDERS) {
        printf("Order storage is full.\n");
        return;
    }

    clearInputBuffer();
    printf("Enter student name: ");
    readLine(newOrder.studentName, sizeof(newOrder.studentName));

    do {
        displayMenu();
        printf("\nEnter item ID: ");
        if (scanf("%d", &id) != 1) {
            clearInputBuffer();
            printf("Invalid input.\n");
            continue;
        }

        index = findMenuItem(id);
        if (index == -1) {
            printf("Item not found.\n");
        } else if (menu[index].stock == 0) {
            printf("This item is out of stock.\n");
        } else {
            printf("Enter quantity: ");
            scanf("%d", &quantity);

            if (quantity <= 0 || quantity > menu[index].stock) {
                printf("Invalid quantity. Available stock: %d\n", menu[index].stock);
            } else {
                cartIndex = -1;
                for (i = 0; i < newOrder.itemCount; i++) {
                    if (newOrder.items[i].itemId == id) {
                        cartIndex = i;
                        break;
                    }
                }

                if (cartIndex != -1) {
                    if (newOrder.items[cartIndex].quantity + quantity > menu[index].stock) {
                        printf("Total quantity exceeds available stock.\n");
                    } else {
                        newOrder.items[cartIndex].quantity += quantity;
                        printf("Cart quantity updated.\n");
                    }
                } else if (newOrder.itemCount >= MAX_CART_ITEMS) {
                    printf("Cart is full.\n");
                } else {
                    newOrder.items[newOrder.itemCount].itemId = menu[index].id;
                    strcpy(newOrder.items[newOrder.itemCount].name, menu[index].name);
                    newOrder.items[newOrder.itemCount].price = menu[index].price;
                    newOrder.items[newOrder.itemCount].quantity = quantity;
                    newOrder.itemCount++;
                    printf("Item added to cart.\n");
                }
            }
        }

        printf("Add another item? (1 = Yes, 0 = No): ");
        scanf("%d", &choice);
    } while (choice == 1);

    if (newOrder.itemCount == 0) {
        printf("No item selected. Order cancelled.\n");
        return;
    }

    newOrder.total = 0;
    for (i = 0; i < newOrder.itemCount; i++) {
        newOrder.total += newOrder.items[i].price * newOrder.items[i].quantity;
    }

    printf("Choose payment mode (1 = Cash, 2 = UPI): ");
    scanf("%d", &choice);
    strcpy(newOrder.paymentMode, choice == 2 ? "UPI" : "Cash");

    newOrder.orderId = orderCount == 0 ? 1001 : orders[orderCount - 1].orderId + 1;
    strcpy(newOrder.status, "Pending");

    for (i = 0; i < newOrder.itemCount; i++) {
        index = findMenuItem(newOrder.items[i].itemId);
        if (index != -1) {
            menu[index].stock -= newOrder.items[i].quantity;
        }
    }

    orders[orderCount++] = newOrder;
    saveData();
    printBill(&newOrder);
    printf("Keep your Order ID for tracking.\n");
}

void trackOrder(void) {
    int orderId, index;
    printf("Enter Order ID: ");
    scanf("%d", &orderId);
    index = findOrder(orderId);
    if (index == -1) {
        printf("Order not found.\n");
    } else {
        printBill(&orders[index]);
    }
}

void cancelOrder(void) {
    int orderId, index, menuIndex, i;
    printf("Enter Order ID to cancel: ");
    scanf("%d", &orderId);
    index = findOrder(orderId);

    if (index == -1) {
        printf("Order not found.\n");
        return;
    }
    if (strcmp(orders[index].status, "Pending") != 0) {
        printf("Only a pending order can be cancelled.\n");
        return;
    }

    for (i = 0; i < orders[index].itemCount; i++) {
        menuIndex = findMenuItem(orders[index].items[i].itemId);
        if (menuIndex != -1) {
            menu[menuIndex].stock += orders[index].items[i].quantity;
        }
    }
    strcpy(orders[index].status, "Cancelled");
    saveData();
    printf("Order cancelled and stock restored.\n");
}

void addMenuItem(void) {
    MenuItem item;
    if (menuCount >= MAX_ITEMS) {
        printf("Menu is full.\n");
        return;
    }

    printf("Enter new item ID: ");
    scanf("%d", &item.id);
    if (findMenuItem(item.id) != -1) {
        printf("This ID already exists.\n");
        return;
    }
    clearInputBuffer();
    printf("Enter item name: ");
    readLine(item.name, sizeof(item.name));
    printf("Enter price: ");
    scanf("%f", &item.price);
    printf("Enter stock: ");
    scanf("%d", &item.stock);

    if (item.price < 0 || item.stock < 0) {
        printf("Price and stock cannot be negative.\n");
        return;
    }
    menu[menuCount++] = item;
    saveData();
    printf("Item added successfully.\n");
}

void updateMenuItem(void) {
    int id, index, choice, amount;
    float price;
    printf("Enter item ID: ");
    scanf("%d", &id);
    index = findMenuItem(id);
    if (index == -1) {
        printf("Item not found.\n");
        return;
    }

    printf("1. Change price\n2. Add stock\nEnter choice: ");
    scanf("%d", &choice);
    if (choice == 1) {
        printf("Enter new price: ");
        scanf("%f", &price);
        if (price >= 0) {
            menu[index].price = price;
        }
    } else if (choice == 2) {
        printf("Enter stock quantity to add: ");
        scanf("%d", &amount);
        if (amount > 0) {
            menu[index].stock += amount;
        }
    } else {
        printf("Invalid choice.\n");
        return;
    }
    saveData();
    printf("Item updated successfully.\n");
}

void viewAllOrders(void) {
    int i;
    if (orderCount == 0) {
        printf("No orders available.\n");
        return;
    }
    printf("\n%-10s %-22s %-12s %-12s\n", "Order ID", "Student", "Total", "Status");
    for (i = 0; i < orderCount; i++) {
        printf("%-10d %-22s Rs.%-9.2f %-12s\n",
               orders[i].orderId, orders[i].studentName,
               orders[i].total, orders[i].status);
    }
}

void updateOrderStatus(void) {
    int orderId, index, choice;
    printf("Enter Order ID: ");
    scanf("%d", &orderId);
    index = findOrder(orderId);
    if (index == -1) {
        printf("Order not found.\n");
        return;
    }
    if (strcmp(orders[index].status, "Cancelled") == 0) {
        printf("A cancelled order cannot be updated.\n");
        return;
    }

    printf("1. Preparing\n2. Ready\n3. Collected\nEnter new status: ");
    scanf("%d", &choice);
    if (choice == 1) {
        strcpy(orders[index].status, "Preparing");
    } else if (choice == 2) {
        strcpy(orders[index].status, "Ready");
    } else if (choice == 3) {
        strcpy(orders[index].status, "Collected");
    } else {
        printf("Invalid choice.\n");
        return;
    }
    saveData();
    printf("Order status updated.\n");
}

void dailySalesReport(void) {
    int i, completed = 0;
    float sales = 0;
    for (i = 0; i < orderCount; i++) {
        if (strcmp(orders[i].status, "Collected") == 0) {
            completed++;
            sales += orders[i].total;
        }
    }
    printf("\n========== SALES REPORT ==========\n");
    printf("Total orders stored : %d\n", orderCount);
    printf("Completed orders    : %d\n", completed);
    printf("Completed sales     : Rs. %.2f\n", sales);
    printf("==================================\n");
}

void studentPanel(void) {
    int choice;
    do {
        printf("\n========== STUDENT PANEL ==========\n");
        printf("1. View menu\n2. Place order\n3. Track order\n");
        printf("4. Cancel pending order\n0. Back\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displayMenu(); break;
            case 2: placeOrder(); break;
            case 3: trackOrder(); break;
            case 4: cancelOrder(); break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

void staffPanel(void) {
    int choice, password;
    printf("Enter staff password: ");
    scanf("%d", &password);
    if (password != 1234) {
        printf("Incorrect password.\n");
        return;
    }

    do {
        printf("\n=========== STAFF PANEL ===========\n");
        printf("1. View menu and stock\n2. Add menu item\n");
        printf("3. Update price/stock\n4. View all orders\n");
        printf("5. Update order status\n6. Sales report\n");
        printf("0. Back\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displayMenu(); break;
            case 2: addMenuItem(); break;
            case 3: updateMenuItem(); break;
            case 4: viewAllOrders(); break;
            case 5: updateOrderStatus(); break;
            case 6: dailySalesReport(); break;
            case 0: break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

int main(void) {
    int choice;
    loadData();
    addDefaultMenu();

    do {
        printf("\n====================================\n");
        printf("      CAMPUSBITE - COLLEGE CANTEEN\n");
        printf("====================================\n");
        printf("1. Student panel\n2. Staff panel\n0. Save and exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            clearInputBuffer();
            choice = -1;
        }

        switch (choice) {
            case 1: studentPanel(); break;
            case 2: staffPanel(); break;
            case 0:
                saveData();
                printf("Data saved. Thank you!\n");
                break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
