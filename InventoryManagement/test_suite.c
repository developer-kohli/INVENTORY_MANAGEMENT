/**
 * @file test_suite.c
 * @brief Automated Test Suite verifying all 12 requirements from Step 8.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "inventory.h"
#include "sorting.h"
#include "file.h"

#define TEST_FILE "test_inventory.dat"

static int g_testsPassed = 0;
static int g_testsTotal = 0;

#define RUN_TEST(fn) do { \
    g_testsTotal++; \
    printf("\n[TEST %d] Running %s...", g_testsTotal, #fn); \
    if (fn()) { \
        printf(" -> PASSED [OK]\n"); \
        g_testsPassed++; \
    } else { \
        printf(" -> FAILED [FAIL]\n"); \
    } \
} while(0)

/* Check 11: Handling an Empty Inventory */
static bool test_empty_inventory(void) {
    Inventory* inv = inventory_create();
    assert(inv != NULL);
    assert(inv->totalProducts == 0);
    assert(inv->head == NULL);

    /* Search on empty returns NULL */
    assert(inventory_search_product(inv, 100) == NULL);

    /* Delete on empty returns false */
    assert(!inventory_delete_product(inv, 100));

    /* Restock / Sell on empty fails */
    char err[64];
    assert(!inventory_restock_product(inv, 100, 10, err, sizeof(err)));
    assert(!inventory_sell_product(inv, 100, 5, NULL, err, sizeof(err)));

    inventory_free(inv);
    return true;
}

/* Check 1 & Check 7: Adding multiple products & handling invalid input */
static bool test_add_products_and_validation(void) {
    Inventory* inv = inventory_create();
    char err[128];

    /* Invalid: ID <= 0 */
    Product pBad = {0, "Bad ID", "Electronics", 10, 19.99};
    assert(!inventory_add_product(inv, pBad, err, sizeof(err)));

    /* Invalid: Empty name */
    Product pBadName = {1, "", "Electronics", 10, 19.99};
    assert(!inventory_add_product(inv, pBadName, err, sizeof(err)));

    /* Invalid: Negative quantity */
    Product pBadQty = {2, "Negative Qty", "Electronics", -5, 19.99};
    assert(!inventory_add_product(inv, pBadQty, err, sizeof(err)));

    /* Invalid: Zero/Negative price */
    Product pBadPrice = {3, "Zero Price", "Electronics", 5, 0.0};
    assert(!inventory_add_product(inv, pBadPrice, err, sizeof(err)));

    /* Valid Products */
    Product p1 = {101, "Dell XPS 15 Laptop", "Laptops", 8, 1299.99};
    Product p2 = {102, "Logitech MX Master 3", "Accessories", 25, 99.50};
    Product p3 = {103, "LG UltraFine 4K Monitor", "Monitors", 4, 499.00};
    Product p4 = {104, "Mechanical Keyboard", "Accessories", 2, 79.99};

    assert(inventory_add_product(inv, p1, err, sizeof(err)));
    assert(inventory_add_product(inv, p2, err, sizeof(err)));
    assert(inventory_add_product(inv, p3, err, sizeof(err)));
    assert(inventory_add_product(inv, p4, err, sizeof(err)));
    assert(inv->totalProducts == 4);

    inventory_free(inv);
    return true;
}

/* Check 6: Handling duplicate product IDs */
static bool test_duplicate_product_id(void) {
    Inventory* inv = inventory_create();
    char err[128];

    Product p1 = {50, "USB-C Cable", "Cables", 50, 9.99};
    assert(inventory_add_product(inv, p1, err, sizeof(err)));

    /* Attempt duplicate ID 50 with different name */
    Product pDup = {50, "Duplicate Cable", "Cables", 20, 12.99};
    bool res = inventory_add_product(inv, pDup, err, sizeof(err));
    assert(!res);
    assert(inv->totalProducts == 1);

    inventory_free(inv);
    return true;
}

/* Check 2 & Check 3: Searching for existing and nonexistent products */
static bool test_search_products(void) {
    Inventory* inv = inventory_create();
    char err[64];

    Product p1 = {201, "iPad Air", "Tablets", 15, 599.99};
    Product p2 = {202, "Apple Pencil", "Accessories", 30, 129.00};
    inventory_add_product(inv, p1, err, sizeof(err));
    inventory_add_product(inv, p2, err, sizeof(err));

    /* Existing */
    ProductNode* node1 = inventory_search_product(inv, 201);
    assert(node1 != NULL);
    assert(strcmp(node1->product.name, "iPad Air") == 0);
    assert(node1->product.quantity == 15);

    ProductNode* node2 = inventory_search_product(inv, 202);
    assert(node2 != NULL);
    assert(strcmp(node2->product.name, "Apple Pencil") == 0);

    /* Nonexistent */
    ProductNode* nodeNon = inventory_search_product(inv, 999);
    assert(nodeNon == NULL);

    inventory_free(inv);
    return true;
}

/* Check 4 & Check 5: Updating product details and deleting products */
static bool test_update_and_delete_product(void) {
    Inventory* inv = inventory_create();
    char err[128];

    Product p1 = {301, "Original Mouse", "Accessories", 10, 25.00};
    inventory_add_product(inv, p1, err, sizeof(err));

    /* Update details */
    bool updated = inventory_update_product(inv, 301, "Ergonomic Mouse Pro", "Peripherals", 18, 35.50, err, sizeof(err));
    assert(updated);

    ProductNode* node = inventory_search_product(inv, 301);
    assert(node != NULL);
    assert(strcmp(node->product.name, "Ergonomic Mouse Pro") == 0);
    assert(strcmp(node->product.category, "Peripherals") == 0);
    assert(node->product.quantity == 18);
    assert(node->product.price == 35.50);

    /* Delete product */
    bool deleted = inventory_delete_product(inv, 301);
    assert(deleted);
    assert(inv->totalProducts == 0);
    assert(inv->head == NULL);

    /* Confirm it cannot be searched anymore */
    assert(inventory_search_product(inv, 301) == NULL);

    inventory_free(inv);
    return true;
}

/* Restock and Sell operations */
static bool test_restock_and_sell(void) {
    Inventory* inv = inventory_create();
    char err[128];

    Product p = {401, "Gaming Headset", "Audio", 10, 80.00};
    inventory_add_product(inv, p, err, sizeof(err));

    /* Restock */
    assert(inventory_restock_product(inv, 401, 15, err, sizeof(err)));
    ProductNode* n = inventory_search_product(inv, 401);
    assert(n->product.quantity == 25);

    /* Sell valid amount */
    double bill = 0.0;
    assert(inventory_sell_product(inv, 401, 5, &bill, err, sizeof(err)));
    assert(n->product.quantity == 20);
    assert(bill == 400.00); /* 5 * 80 */

    /* Attempt to sell more than available */
    assert(!inventory_sell_product(inv, 401, 25, &bill, err, sizeof(err)));
    assert(n->product.quantity == 20); /* Unchanged */

    inventory_free(inv);
    return true;
}

/* Check 12: Hash Table Collisions & Separate Chaining */
static bool test_hash_table_collisions(void) {
    Inventory* inv = inventory_create();
    char err[128];

    /* Table size is 101.
     * IDs 1, 102, and 203 all have (id % 101) == 1.
     * This creates a 3-element collision chain in bucket index 1.
     */
    assert(hashtable_hash(1) == 1);
    assert(hashtable_hash(102) == 1);
    assert(hashtable_hash(203) == 1);

    Product p1 = {1, "Item Alpha", "CatA", 10, 15.00};
    Product p2 = {102, "Item Beta", "CatB", 20, 25.00};
    Product p3 = {203, "Item Gamma", "CatC", 30, 35.00};

    assert(inventory_add_product(inv, p1, err, sizeof(err)));
    assert(inventory_add_product(inv, p2, err, sizeof(err)));
    assert(inventory_add_product(inv, p3, err, sizeof(err)));
    assert(inv->totalProducts == 3);

    /* Verify all three distinct collided items can be searched independently */
    ProductNode* f1 = inventory_search_product(inv, 1);
    ProductNode* f2 = inventory_search_product(inv, 102);
    ProductNode* f3 = inventory_search_product(inv, 203);

    assert(f1 != NULL && strcmp(f1->product.name, "Item Alpha") == 0);
    assert(f2 != NULL && strcmp(f2->product.name, "Item Beta") == 0);
    assert(f3 != NULL && strcmp(f3->product.name, "Item Gamma") == 0);

    /* Delete the middle collided item (102) */
    assert(inventory_delete_product(inv, 102));
    assert(inventory_search_product(inv, 102) == NULL);

    /* Confirm 1 and 203 still remain searchable in the collision chain! */
    assert(inventory_search_product(inv, 1) != NULL);
    assert(inventory_search_product(inv, 203) != NULL);

    inventory_free(inv);
    return true;
}

/* Check 8: Sorting Products with Merge Sort */
static bool test_merge_sort_algorithms(void) {
    Inventory* inv = inventory_create();
    char err[128];

    Product p1 = {501, "Zebra Printer", "Office", 12, 350.00};
    Product p2 = {502, "Apple Watch", "Wearables", 40, 399.00};
    Product p3 = {503, "Microsoft Surface", "Laptops", 3, 899.00};
    Product p4 = {504, "Dell Monitor", "Monitors", 22, 199.00};

    inventory_add_product(inv, p1, err, sizeof(err));
    inventory_add_product(inv, p2, err, sizeof(err));
    inventory_add_product(inv, p3, err, sizeof(err));
    inventory_add_product(inv, p4, err, sizeof(err));

    /* 1. Sort by Name Ascending: Apple Watch -> Dell Monitor -> Microsoft Surface -> Zebra Printer */
    inv->head = merge_sort(inv->head, SORT_BY_NAME, ORDER_ASCENDING);
    assert(strcmp(inv->head->product.name, "Apple Watch") == 0);
    assert(strcmp(inv->head->next->product.name, "Dell Monitor") == 0);
    assert(strcmp(inv->head->next->next->product.name, "Microsoft Surface") == 0);
    assert(strcmp(inv->head->next->next->next->product.name, "Zebra Printer") == 0);

    /* 2. Sort by Price Ascending: 199.00 -> 350.00 -> 399.00 -> 899.00 */
    inv->head = merge_sort(inv->head, SORT_BY_PRICE, ORDER_ASCENDING);
    assert(inv->head->product.price == 199.00);
    assert(inv->head->next->product.price == 350.00);
    assert(inv->head->next->next->product.price == 399.00);
    assert(inv->head->next->next->next->product.price == 899.00);

    /* 3. Sort by Quantity Descending: 40 -> 22 -> 12 -> 3 */
    inv->head = merge_sort(inv->head, SORT_BY_QUANTITY, ORDER_DESCENDING);
    assert(inv->head->product.quantity == 40);
    assert(inv->head->next->product.quantity == 22);
    assert(inv->head->next->next->product.quantity == 12);
    assert(inv->head->next->next->next->product.quantity == 3);

    /* Hash table search still works seamlessly after merge sorting */
    assert(inventory_search_product(inv, 501) != NULL);
    assert(inventory_search_product(inv, 502) != NULL);
    assert(inventory_search_product(inv, 503) != NULL);
    assert(inventory_search_product(inv, 504) != NULL);

    inventory_free(inv);
    return true;
}

/* Check 9: Low Stock Alerts */
static bool test_low_stock_threshold(void) {
    Inventory* inv = inventory_create();
    char err[128];

    Product pNormal = {601, "Normal Stock Item", "General", 10, 10.00};
    Product pLow = {602, "Low Stock Item", "General", 4, 10.00};
    Product pOut = {603, "Out of Stock Item", "General", 0, 10.00};

    inventory_add_product(inv, pNormal, err, sizeof(err));
    inventory_add_product(inv, pLow, err, sizeof(err));
    inventory_add_product(inv, pOut, err, sizeof(err));

    /* Count items with quantity < 5 */
    int lowCount = 0;
    ProductNode* curr = inv->head;
    while (curr) {
        if (curr->product.quantity < LOW_STOCK_THRESHOLD) {
            lowCount++;
        }
        curr = curr->next;
    }
    assert(lowCount == 2); /* 4 units and 0 units */

    inventory_free(inv);
    return true;
}

/* Check 10: Saving and Loading Records from Binary File */
static bool test_file_persistence(void) {
    /* 1. Save data */
    Inventory* inv1 = inventory_create();
    char err[128];

    Product p1 = {701, "Persisted Laptop", "Tech", 15, 1200.00};
    Product p2 = {702, "Persisted Phone", "Tech", 8, 800.00};
    inventory_add_product(inv1, p1, err, sizeof(err));
    inventory_add_product(inv1, p2, err, sizeof(err));

    bool saved = save_inventory_to_file(inv1, TEST_FILE);
    assert(saved);
    inventory_free(inv1);

    /* 2. Load into fresh inventory */
    Inventory* inv2 = inventory_create();
    bool loaded = load_inventory_from_file(inv2, TEST_FILE);
    assert(loaded);
    assert(inv2->totalProducts == 2);

    ProductNode* l1 = inventory_search_product(inv2, 701);
    assert(l1 != NULL);
    assert(strcmp(l1->product.name, "Persisted Laptop") == 0);
    assert(l1->product.quantity == 15);
    assert(l1->product.price == 1200.00);

    ProductNode* l2 = inventory_search_product(inv2, 702);
    assert(l2 != NULL);
    assert(strcmp(l2->product.name, "Persisted Phone") == 0);
    assert(l2->product.quantity == 8);

    inventory_free(inv2);

    /* Clean up test file */
    remove(TEST_FILE);
    return true;
}

int main(void) {
    printf("============================================================\n");
    printf("       INVENTORY MANAGEMENT SYSTEM - AUTOMATED TEST SUITE    \n");
    printf("============================================================\n");

    RUN_TEST(test_empty_inventory);
    RUN_TEST(test_add_products_and_validation);
    RUN_TEST(test_duplicate_product_id);
    RUN_TEST(test_search_products);
    RUN_TEST(test_update_and_delete_product);
    RUN_TEST(test_restock_and_sell);
    RUN_TEST(test_hash_table_collisions);
    RUN_TEST(test_merge_sort_algorithms);
    RUN_TEST(test_low_stock_threshold);
    RUN_TEST(test_file_persistence);

    printf("\n============================================================\n");
    printf("TEST RESULTS: %d / %d Tests Passed (%.1f%%)\n",
           g_testsPassed, g_testsTotal, (float)g_testsPassed * 100.0f / (float)g_testsTotal);
    printf("============================================================\n");

    return (g_testsPassed == g_testsTotal) ? EXIT_SUCCESS : EXIT_FAILURE;
}
