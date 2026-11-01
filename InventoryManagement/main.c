/**
 * @file main.c
 * @brief Interactive Console Entry Point for the Inventory Management System.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"
#include "sorting.h"
#include "file.h"

static void print_banner(void) {
    printf("\n==================================================\n");
    printf("========== INVENTORY MANAGEMENT SYSTEM ===========\n");
    printf("==================================================\n");
    printf("  1. Add Product\n");
    printf("  2. Display All Products\n");
    printf("  3. Search Product\n");
    printf("  4. Update Product\n");
    printf("  5. Delete Product\n");
    printf("  6. Restock Product\n");
    printf("  7. Sell Product\n");
    printf("  8. Sort Inventory\n");
    printf("  9. Low Stock Alert\n");
    printf(" 10. Save Inventory\n");
    printf(" 11. Exit\n");
    printf("==================================================\n");
}

/* Handler: Add Product */
static void handle_add_product(Inventory* inv) {
    printf("\n--- [1] ADD NEW PRODUCT ---\n");

    int id = 0;
    if (!read_int("Enter Product ID (positive integer): ", &id)) return;
    if (id <= 0) {
        printf("  [Error] Product ID must be positive (> 0).\n");
        return;
    }

    /* Fast O(1) duplicate check before asking for remaining data */
    if (inventory_search_product(inv, id) != NULL) {
        printf("  [Error] Product ID %d already exists in the inventory! Duplicates are not allowed.\n", id);
        return;
    }

    Product p;
    p.id = id;

    read_string("Enter Product Name: ", p.name, sizeof(p.name));
    read_string("Enter Category: ", p.category, sizeof(p.category));

    if (!read_int("Enter Initial Quantity (>= 0): ", &p.quantity)) return;
    if (p.quantity < 0) {
        printf("  [Error] Quantity cannot be negative.\n");
        return;
    }

    if (!read_double("Enter Unit Price ($ > 0.00): ", &p.price)) return;
    if (p.price <= 0.0) {
        printf("  [Error] Price must be greater than zero.\n");
        return;
    }

    char errorMsg[128];
    if (inventory_add_product(inv, p, errorMsg, sizeof(errorMsg))) {
        printf("  [Success] Product '%s' (ID: %d) successfully added to inventory.\n", p.name, p.id);
        /* Auto-save as per Step 5 requirements */
        save_inventory_to_file(inv, DEFAULT_DATA_FILE);
    } else {
        printf("  [Error] Failed to add product: %s\n", errorMsg);
    }
}

/* Handler: Display Products (with Category Filter and Statistics sub-options) */
static void handle_display_products(const Inventory* inv) {
    if (inv->head == NULL) {
        inventory_display_table(NULL);
        return;
    }

    printf("\n--- [2] DISPLAY INVENTORY ---\n");
    printf("1. View All Products (Full Table)\n");
    printf("2. Filter by Category\n");
    printf("3. View Inventory Statistics\n");
    printf("4. Return to Main Menu\n");
    int choice = 1;
    if (!read_int("Select display mode (1-4): ", &choice)) return;

    switch (choice) {
        case 1:
            inventory_display_table(inv->head);
            break;
        case 2: {
            char category[MAX_CAT_LEN];
            read_string("Enter category name to filter by: ", category, sizeof(category));
            inventory_display_by_category(inv, category);
            break;
        }
        case 3:
            inventory_display_statistics(inv);
            break;
        case 4:
        default:
            break;
    }
}

/* Handler: Search Product */
static void handle_search_product(const Inventory* inv) {
    if (inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. No products to search.\n");
        return;
    }

    printf("\n--- [3] SEARCH PRODUCT BY ID (HASH TABLE) ---\n");
    int id = 0;
    if (!read_int("Enter Product ID to search: ", &id)) return;

    /* O(1) Hash Table Lookup */
    ProductNode* found = inventory_search_product(inv, id);
    if (found) {
        printf("  [Found] Product located via Hash Table:\n");
        inventory_display_product_card(&found->product);
    } else {
        printf("  [Error] No product found with ID %d.\n", id);
    }
}

/* Handler: Update Product */
static void handle_update_product(Inventory* inv) {
    if (inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. Nothing to update.\n");
        return;
    }

    printf("\n--- [4] UPDATE PRODUCT DETAILS ---\n");
    int id = 0;
    if (!read_int("Enter Product ID to update: ", &id)) return;

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        printf("  [Error] Product ID %d not found.\n", id);
        return;
    }

    printf("\nCurrent Details:\n");
    inventory_display_product_card(&target->product);

    char name[MAX_NAME_LEN];
    char category[MAX_CAT_LEN];
    int quantity = 0;
    double price = 0.0;

    printf("\nUpdating Product [%d] (Enter new values):\n", id);
    read_string("Enter New Product Name: ", name, sizeof(name));
    read_string("Enter New Category: ", category, sizeof(category));

    if (!read_int("Enter New Quantity (>= 0): ", &quantity)) return;
    if (quantity < 0) {
        printf("  [Error] Quantity cannot be negative.\n");
        return;
    }

    if (!read_double("Enter New Unit Price ($ > 0.00): ", &price)) return;
    if (price <= 0.0) {
        printf("  [Error] Price must be greater than zero.\n");
        return;
    }

    char errorMsg[128];
    if (inventory_update_product(inv, id, name, category, quantity, price, errorMsg, sizeof(errorMsg))) {
        printf("  [Success] Product ID %d successfully updated.\n", id);
        save_inventory_to_file(inv, DEFAULT_DATA_FILE);
    } else {
        printf("  [Error] Update failed: %s\n", errorMsg);
    }
}

/* Handler: Delete Product */
static void handle_delete_product(Inventory* inv) {
    if (inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. Nothing to delete.\n");
        return;
    }

    printf("\n--- [5] DELETE PRODUCT ---\n");
    int id = 0;
    if (!read_int("Enter Product ID to delete: ", &id)) return;

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        printf("  [Error] Product ID %d not found.\n", id);
        return;
    }

    inventory_display_product_card(&target->product);

    char confirm[16];
    read_string("Are you sure you want to permanently delete this product? (y/n): ", confirm, sizeof(confirm));
    if (confirm[0] == 'y' || confirm[0] == 'Y') {
        if (inventory_delete_product(inv, id)) {
            printf("  [Success] Product ID %d successfully deleted from system.\n", id);
            save_inventory_to_file(inv, DEFAULT_DATA_FILE);
        } else {
            printf("  [Error] Failed to delete product ID %d.\n", id);
        }
    } else {
        printf("  [Cancelled] Deletion aborted.\n");
    }
}

/* Handler: Restock Product */
static void handle_restock_product(Inventory* inv) {
    if (inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. No products available to restock.\n");
        return;
    }

    printf("\n--- [6] RESTOCK PRODUCT ---\n");
    int id = 0;
    if (!read_int("Enter Product ID to restock: ", &id)) return;

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        printf("  [Error] Product ID %d not found.\n", id);
        return;
    }

    printf("Product '%s' current stock: %d units.\n", target->product.name, target->product.quantity);

    int addQty = 0;
    if (!read_int("Enter quantity to add (> 0): ", &addQty)) return;

    char errorMsg[128];
    if (inventory_restock_product(inv, id, addQty, errorMsg, sizeof(errorMsg))) {
        printf("  [Success] Restocked %d units. New Total Stock: %d units.\n",
               addQty, target->product.quantity);
        save_inventory_to_file(inv, DEFAULT_DATA_FILE);
    } else {
        printf("  [Error] Restock failed: %s\n", errorMsg);
    }
}

/* Handler: Sell Product */
static void handle_sell_product(Inventory* inv) {
    if (inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. No products available for sale.\n");
        return;
    }

    printf("\n--- [7] SELL PRODUCT ---\n");
    int id = 0;
    if (!read_int("Enter Product ID to sell: ", &id)) return;

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        printf("  [Error] Product ID %d not found.\n", id);
        return;
    }

    printf("Product '%s' | Available: %d units | Unit Price: $%.2f\n",
           target->product.name, target->product.quantity, target->product.price);

    if (target->product.quantity == 0) {
        printf("  [Error] Item is currently OUT OF STOCK!\n");
        return;
    }

    int sellQty = 0;
    if (!read_int("Enter quantity to sell (> 0): ", &sellQty)) return;

    double totalBill = 0.0;
    char errorMsg[128];
    if (inventory_sell_product(inv, id, sellQty, &totalBill, errorMsg, sizeof(errorMsg))) {
        printf("\n+---------------- TRANSACTION RECEIPT ----------------+\n");
        printf("| Product Sold     : %-32.32s |\n", target->product.name);
        printf("| Units Sold       : %-32d |\n", sellQty);
        printf("| Unit Price       : $%-31.2f |\n", target->product.price);
        printf("| Total Sale Value : $%-31.2f |\n", totalBill);
        printf("| Remaining Stock  : %-32d |\n", target->product.quantity);
        printf("+-----------------------------------------------------+\n");
        save_inventory_to_file(inv, DEFAULT_DATA_FILE);
    } else {
        printf("  [Error] Sale transaction failed: %s\n", errorMsg);
    }
}

/* Handler: Save Inventory */
static void handle_save_inventory(const Inventory* inv) {
    printf("\n--- [10] SAVE INVENTORY TO FILE ---\n");
    if (save_inventory_to_file(inv, DEFAULT_DATA_FILE)) {
        printf("  [Success] %d inventory record(s) safely saved to '%s'.\n",
               inv->totalProducts, DEFAULT_DATA_FILE);
    } else {
        printf("  [Error] Failed to save inventory to '%s'.\n", DEFAULT_DATA_FILE);
    }
}

int main(void) {
    /* Initialize Inventory System */
    Inventory* inv = inventory_create();
    if (!inv) {
        fprintf(stderr, "Fatal error: Could not initialize inventory system.\n");
        return EXIT_FAILURE;
    }

    printf("\n=======================================================\n");
    printf("   WELCOME TO DATA STRUCTURES INVENTORY MANAGEMENT     \n");
    printf("=======================================================\n");

    /* Step 5 Requirement: Automatically load records upon startup */
    load_inventory_from_file(inv, DEFAULT_DATA_FILE);

    int choice = 0;
    bool running = true;

    while (running) {
        print_banner();
        if (!read_int("Enter your choice (1-11): ", &choice)) {
            printf("Error reading choice. Exiting application.\n");
            break;
        }

        switch (choice) {
            case 1:
                handle_add_product(inv);
                break;
            case 2:
                handle_display_products(inv);
                break;
            case 3:
                handle_search_product(inv);
                break;
            case 4:
                handle_update_product(inv);
                break;
            case 5:
                handle_delete_product(inv);
                break;
            case 6:
                handle_restock_product(inv);
                break;
            case 7:
                handle_sell_product(inv);
                break;
            case 8:
                inventory_sort_menu(inv);
                break;
            case 9:
                inventory_display_low_stock(inv);
                break;
            case 10:
                handle_save_inventory(inv);
                break;
            case 11:
                printf("\nSaving records before exit...\n");
                save_inventory_to_file(inv, DEFAULT_DATA_FILE);
                printf("All changes preserved. Thank you for using Inventory Management System!\n");
                running = false;
                break;
            default:
                printf("\n[Error] Invalid choice '%d'. Please select a valid option from 1 to 11.\n", choice);
                break;
        }
    }

    /* Clean up all dynamically allocated memory */
    inventory_free(inv);
    return EXIT_SUCCESS;
}
