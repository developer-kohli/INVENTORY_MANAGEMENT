/**
 * @file inventory.c
 * @brief Implementation of Inventory Management System data structures and core operations.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "inventory.h"

/* Case-insensitive string comparison helper (portable across Windows & POSIX) */
static int case_insensitive_compare(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/* ========================================================================= */
/*                           HASH TABLE OPERATIONS                           */
/* ========================================================================= */

/**
 * @brief Allocates and initializes an empty Hash Table.
 * @return Pointer to allocated HashTable, or NULL on memory failure.
 */
HashTable* hashtable_create(void) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) {
        perror("Failed to allocate memory for HashTable");
        return NULL;
    }
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        ht->buckets[i] = NULL;
    }
    ht->count = 0;
    return ht;
}

/**
 * @brief Frees all nodes in the Hash Table collision chains and the table itself.
 * Note: Does not free the underlying ProductNodes, as they belong to the linked list.
 */
void hashtable_free(HashTable* ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        HashNode* curr = ht->buckets[i];
        while (curr != NULL) {
            HashNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
        ht->buckets[i] = NULL;
    }
    free(ht);
}

/**
 * @brief Hash function mapping an integer Product ID to an index in [0, HASH_TABLE_SIZE - 1].
 * Uses modular arithmetic with prime table size for uniform key distribution.
 */
unsigned int hashtable_hash(int id) {
    long long safeId = (long long)id;
    if (safeId < 0) safeId = -safeId;
    return (unsigned int)(safeId % HASH_TABLE_SIZE);
}

/**
 * @brief Inserts a ProductNode pointer into the Hash Table using separate chaining.
 * @param ht Target Hash Table
 * @param id Product ID key
 * @param pNode Pointer to the linked list node
 * @return true on success, false on failure (e.g. out of memory)
 */
bool hashtable_insert(HashTable* ht, int id, ProductNode* pNode) {
    if (!ht || !pNode) return false;

    unsigned int index = hashtable_hash(id);

    /* Allocate a new collision chain node */
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    if (!newNode) {
        perror("Failed to allocate memory for HashNode");
        return false;
    }
    newNode->id = id;
    newNode->pNode = pNode;

    /* Prepend to bucket list (O(1) insertion) */
    newNode->next = ht->buckets[index];
    ht->buckets[index] = newNode;
    ht->count++;
    return true;
}

/**
 * @brief Searches for a product by its ID in the Hash Table.
 * @param ht Hash Table
 * @param id Product ID to find
 * @return Pointer to ProductNode in linked list if found, NULL otherwise.
 */
ProductNode* hashtable_search(const HashTable* ht, int id) {
    if (!ht) return NULL;

    unsigned int index = hashtable_hash(id);
    HashNode* curr = ht->buckets[index];

    /* Traverse collision chain */
    while (curr != NULL) {
        if (curr->id == id) {
            return curr->pNode;
        }
        curr = curr->next;
    }
    return NULL;
}

/**
 * @brief Deletes a key from the Hash Table bucket collision chain.
 * @param ht Hash Table
 * @param id Product ID to delete
 * @return true if deleted, false if not found
 */
bool hashtable_delete(HashTable* ht, int id) {
    if (!ht) return false;

    unsigned int index = hashtable_hash(id);
    HashNode* curr = ht->buckets[index];
    HashNode* prev = NULL;

    while (curr != NULL) {
        if (curr->id == id) {
            if (prev == NULL) {
                ht->buckets[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            ht->count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

/* ========================================================================= */
/*                         INVENTORY LIFECYCLE                               */
/* ========================================================================= */

/**
 * @brief Initializes the complete Inventory system (Linked List + Hash Table).
 */
Inventory* inventory_create(void) {
    Inventory* inv = (Inventory*)malloc(sizeof(Inventory));
    if (!inv) {
        perror("Failed to allocate memory for Inventory");
        return NULL;
    }
    inv->head = NULL;
    inv->table = hashtable_create();
    if (!inv->table) {
        free(inv);
        return NULL;
    }
    inv->totalProducts = 0;
    return inv;
}

/**
 * @brief Frees all memory associated with the inventory (Nodes, Hash Table, and Container).
 */
void inventory_free(Inventory* inv) {
    if (!inv) return;

    /* Free linked list */
    ProductNode* curr = inv->head;
    while (curr != NULL) {
        ProductNode* next = curr->next;
        free(curr);
        curr = next;
    }
    inv->head = NULL;

    /* Free hash table */
    if (inv->table) {
        hashtable_free(inv->table);
        inv->table = NULL;
    }

    free(inv);
}

/* ========================================================================= */
/*                         CORE INVENTORY OPERATIONS                         */
/* ========================================================================= */

/**
 * @brief Adds a new product to the inventory.
 * Validates inputs, ensures unique ID via Hash Table, appends to Linked List,
 * and indexes into the Hash Table.
 */
bool inventory_add_product(Inventory* inv, Product prod, char* errorMsg, size_t errorSize) {
    if (!inv) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Inventory pointer is NULL.");
        return false;
    }

    /* Validation: Product ID must be positive */
    if (prod.id <= 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Product ID must be a positive integer.");
        return false;
    }

    /* Validation: Product Name cannot be empty */
    if (strlen(prod.name) == 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Product name cannot be empty.");
        return false;
    }

    /* Validation: Category cannot be empty */
    if (strlen(prod.category) == 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Category cannot be empty.");
        return false;
    }

    /* Validation: Quantity cannot be negative */
    if (prod.quantity < 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Quantity cannot be negative.");
        return false;
    }

    /* Validation: Price must be positive */
    if (prod.price <= 0.0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Price must be greater than zero.");
        return false;
    }

    /* Validation: Prevent duplicate ID using Hash Table O(1) check */
    if (hashtable_search(inv->table, prod.id) != NULL) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Duplicate Error: Product ID %d already exists.", prod.id);
        return false;
    }

    /* Allocate Linked List Node */
    ProductNode* newNode = (ProductNode*)malloc(sizeof(ProductNode));
    if (!newNode) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Memory allocation error for new product node.");
        return false;
    }
    newNode->product = prod;
    newNode->next = NULL;

    /* Append to Linked List to maintain insertion order */
    if (inv->head == NULL) {
        inv->head = newNode;
    } else {
        ProductNode* temp = inv->head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }

    /* Insert into Hash Table for fast O(1) ID lookup */
    if (!hashtable_insert(inv->table, prod.id, newNode)) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Failed to index product in hash table.");
        return false;
    }

    inv->totalProducts++;
    return true;
}

/**
 * @brief Searches for a product by its ID using the Hash Table.
 * @return Pointer to ProductNode, or NULL if not found.
 */
ProductNode* inventory_search_product(const Inventory* inv, int id) {
    if (!inv || !inv->table) return NULL;
    return hashtable_search(inv->table, id);
}

/**
 * @brief Updates details of an existing product.
 */
bool inventory_update_product(Inventory* inv, int id, const char* name, const char* category,
                              int quantity, double price, char* errorMsg, size_t errorSize) {
    if (!inv) return false;

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Product with ID %d not found.", id);
        return false;
    }

    if (quantity < 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Quantity cannot be negative.");
        return false;
    }

    if (price <= 0.0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Price must be greater than zero.");
        return false;
    }

    if (name && strlen(name) > 0) {
        strncpy(target->product.name, name, MAX_NAME_LEN - 1);
        target->product.name[MAX_NAME_LEN - 1] = '\0';
    }

    if (category && strlen(category) > 0) {
        strncpy(target->product.category, category, MAX_CAT_LEN - 1);
        target->product.category[MAX_CAT_LEN - 1] = '\0';
    }

    target->product.quantity = quantity;
    target->product.price = price;
    return true;
}

/**
 * @brief Deletes a product from both the Linked List and Hash Table.
 */
bool inventory_delete_product(Inventory* inv, int id) {
    if (!inv || inv->head == NULL) return false;

    /* Verify existence via Hash Table */
    ProductNode* target = inventory_search_product(inv, id);
    if (!target) return false;

    /* 1. Remove from Hash Table */
    hashtable_delete(inv->table, id);

    /* 2. Remove from Linked List */
    if (inv->head == target) {
        inv->head = target->next;
    } else {
        ProductNode* prev = inv->head;
        while (prev != NULL && prev->next != target) {
            prev = prev->next;
        }
        if (prev != NULL) {
            prev->next = target->next;
        }
    }

    free(target);
    inv->totalProducts--;
    return true;
}

/**
 * @brief Restocks product quantity by a positive amount.
 */
bool inventory_restock_product(Inventory* inv, int id, int addQty, char* errorMsg, size_t errorSize) {
    if (!inv) return false;

    if (addQty <= 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Restock quantity must be greater than zero.");
        return false;
    }

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Product with ID %d was not found.", id);
        return false;
    }

    target->product.quantity += addQty;
    return true;
}

/**
 * @brief Sells a product by deducting quantity and calculating the total bill.
 */
bool inventory_sell_product(Inventory* inv, int id, int sellQty, double* totalCost, char* errorMsg, size_t errorSize) {
    if (!inv) return false;

    if (sellQty <= 0) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Sale quantity must be greater than zero.");
        return false;
    }

    ProductNode* target = inventory_search_product(inv, id);
    if (!target) {
        if (errorMsg) snprintf(errorMsg, errorSize, "Product with ID %d was not found.", id);
        return false;
    }

    if (target->product.quantity < sellQty) {
        if (errorMsg) {
            snprintf(errorMsg, errorSize,
                     "Insufficient Stock! Requested: %d, Available: %d.",
                     sellQty, target->product.quantity);
        }
        return false;
    }

    target->product.quantity -= sellQty;
    if (totalCost) {
        *totalCost = sellQty * target->product.price;
    }
    return true;
}

/* ========================================================================= */
/*                         DISPLAY & REPORTING                               */
/* ========================================================================= */

/**
 * @brief Displays a collection of products in an aligned ASCII table.
 */
void inventory_display_table(const ProductNode* head) {
    if (head == NULL) {
        printf("\n+--------------------------------------------------------------------------+\n");
        printf("|                        [!] INVENTORY IS EMPTY [!]                        |\n");
        printf("+--------------------------------------------------------------------------+\n");
        printf("| No products found in the system. Use Option 1 to add new products.       |\n");
        printf("+--------------------------------------------------------------------------+\n");
        return;
    }

    printf("\n+-----+--------------------------------+--------------------+--------+------------+------------+\n");
    printf("| %-3s | %-30s | %-18s | %-6s | %-10s | %-10s |\n",
           "ID", "Product Name", "Category", "Stock", "Price ($)", "Total ($)");
    printf("+-----+--------------------------------+--------------------+--------+------------+------------+\n");

    const ProductNode* curr = head;
    int count = 0;
    int totalUnits = 0;
    double grandTotal = 0.0;

    while (curr != NULL) {
        const Product* p = &curr->product;
        double itemTotal = p->quantity * p->price;
        totalUnits += p->quantity;
        grandTotal += itemTotal;
        count++;

        printf("| %-3d | %-30.30s | %-18.18s | %6d | %10.2f | %10.2f |\n",
               p->id, p->name, p->category, p->quantity, p->price, itemTotal);

        curr = curr->next;
    }

    printf("+-----+--------------------------------+--------------------+--------+------------+------------+\n");
    printf("| %-54s | %6d |            | %10.2f |\n",
           "SUMMARY: TOTALS", totalUnits, grandTotal);
    printf("+-----------------------------------------------------------+--------+------------+------------+\n");
    printf("  Total Unique Products Listed: %d\n", count);
}

/**
 * @brief Displays a single product in an easy-to-read boxed card format.
 */
void inventory_display_product_card(const Product* p) {
    if (!p) return;
    printf("\n+----------------- PRODUCT DETAILS -----------------+\n");
    printf("| Product ID       : %-30d |\n", p->id);
    printf("| Name             : %-30.30s |\n", p->name);
    printf("| Category         : %-30.30s |\n", p->category);
    printf("| Available Stock  : %-30d |\n", p->quantity);
    printf("| Unit Price ($)   : $%-29.2f |\n", p->price);
    printf("| Inventory Value  : $%-29.2f |\n", p->quantity * p->price);
    printf("| Status           : %-30s |\n",
           p->quantity == 0 ? "OUT OF STOCK" : (p->quantity < LOW_STOCK_THRESHOLD ? "LOW STOCK ALERT!" : "IN STOCK"));
    printf("+---------------------------------------------------+\n");
}

/**
 * @brief Low Stock Alert: Displays all products with stock below threshold (5).
 */
void inventory_display_low_stock(const Inventory* inv) {
    if (!inv || inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. No low-stock items to report.\n");
        return;
    }

    int alertCount = 0;
    const ProductNode* curr = inv->head;

    printf("\n============================= LOW STOCK ALERTS =============================\n");
    printf("Threshold: Quantity < %d\n", LOW_STOCK_THRESHOLD);
    printf("+-----+--------------------------------+--------------------+--------+-------------+\n");
    printf("| %-3s | %-30s | %-18s | %-6s | %-11s |\n",
           "ID", "Product Name", "Category", "Stock", "Status");
    printf("+-----+--------------------------------+--------------------+--------+-------------+\n");

    while (curr != NULL) {
        if (curr->product.quantity < LOW_STOCK_THRESHOLD) {
            alertCount++;
            const char* status = (curr->product.quantity == 0) ? "CRITICAL (0)" : "LOW STOCK";
            printf("| %-3d | %-30.30s | %-18.18s | %6d | %-11s |\n",
                   curr->product.id, curr->product.name, curr->product.category,
                   curr->product.quantity, status);
        }
        curr = curr->next;
    }

    printf("+-----+--------------------------------+--------------------+--------+-------------+\n");
    if (alertCount == 0) {
        printf("  [OK] Excellent! All products have healthy stock levels (>= %d units).\n", LOW_STOCK_THRESHOLD);
    } else {
        printf("  [!] WARNING: %d product(s) require immediate restocking!\n", alertCount);
    }
}

/**
 * @brief Displays comprehensive inventory analytics and statistics.
 */
void inventory_display_statistics(const Inventory* inv) {
    if (!inv || inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. No statistics available.\n");
        return;
    }

    int totalUnique = 0;
    int totalUnits = 0;
    double totalValuation = 0.0;
    int lowStockCount = 0;
    const Product* highestPrice = &inv->head->product;
    const Product* lowestStock = &inv->head->product;

    const ProductNode* curr = inv->head;
    while (curr != NULL) {
        const Product* p = &curr->product;
        totalUnique++;
        totalUnits += p->quantity;
        totalValuation += (p->quantity * p->price);

        if (p->quantity < LOW_STOCK_THRESHOLD) {
            lowStockCount++;
        }
        if (p->price > highestPrice->price) {
            highestPrice = p;
        }
        if (p->quantity < lowestStock->quantity) {
            lowestStock = p;
        }
        curr = curr->next;
    }

    double avgPrice = (totalUnique > 0) ? (totalValuation / (totalUnits > 0 ? totalUnits : 1)) : 0.0;

    printf("\n====================== INVENTORY STATISTICS ======================\n");
    printf("  * Total Unique Product Types : %d\n", totalUnique);
    printf("  * Total Stock Units in Store : %d units\n", totalUnits);
    printf("  * Total Inventory Valuation  : $%.2f\n", totalValuation);
    printf("  * Weighted Avg Price / Unit  : $%.2f\n", avgPrice);
    printf("  * Low Stock Alerts Count     : %d\n", lowStockCount);
    printf("------------------------------------------------------------------\n");
    printf("  * Highest Priced Item        : [%d] %s ($%.2f)\n",
           highestPrice->id, highestPrice->name, highestPrice->price);
    printf("  * Lowest Stock Item          : [%d] %s (%d units left)\n",
           lowestStock->id, lowestStock->name, lowestStock->quantity);
    printf("==================================================================\n");
}

/**
 * @brief Displays all products matching a specific category.
 */
void inventory_display_by_category(const Inventory* inv, const char* category) {
    if (!inv || inv->head == NULL) {
        printf("\n[Notice] Inventory is empty.\n");
        return;
    }

    int matchCount = 0;
    const ProductNode* curr = inv->head;

    printf("\n=================== FILTER BY CATEGORY: \"%s\" ===================\n", category);
    printf("+-----+--------------------------------+--------------------+--------+------------+\n");
    printf("| %-3s | %-30s | %-18s | %-6s | %-10s |\n", "ID", "Product Name", "Category", "Stock", "Price ($)");
    printf("+-----+--------------------------------+--------------------+--------+------------+\n");

    while (curr != NULL) {
        if (case_insensitive_compare(curr->product.category, category) == 0) {
            matchCount++;
            printf("| %-3d | %-30.30s | %-18.18s | %6d | %10.2f |\n",
                   curr->product.id, curr->product.name, curr->product.category,
                   curr->product.quantity, curr->product.price);
        }
        curr = curr->next;
    }

    printf("+-----+--------------------------------+--------------------+--------+------------+\n");
    if (matchCount == 0) {
        printf("  [!] No products found in category '%s'.\n", category);
    } else {
        printf("  Found %d product(s) in category '%s'.\n", matchCount, category);
    }
}

/* ========================================================================= */
/*                         SAFE INPUT UTILITIES                              */
/* ========================================================================= */

/**
 * @brief Removes trailing newline and carriage returns from a string.
 */
void trim_newline(char* str) {
    if (!str) return;
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[len - 1] = '\0';
        len--;
    }
}

/**
 * @brief Safely reads an integer from stdin without leaving garbage in the buffer.
 */
bool read_int(const char* prompt, int* outVal) {
    char buffer[64];
    while (1) {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            return false;
        }
        trim_newline(buffer);
        if (strlen(buffer) == 0) {
            printf("  [Error] Input cannot be blank. Please enter an integer.\n");
            continue;
        }
        char* endPtr;
        long val = strtol(buffer, &endPtr, 10);
        if (*endPtr != '\0') {
            printf("  [Error] Invalid integer '%s'. Please enter numbers only.\n", buffer);
            continue;
        }
        *outVal = (int)val;
        return true;
    }
}

/**
 * @brief Safely reads a floating-point double from stdin.
 */
bool read_double(const char* prompt, double* outVal) {
    char buffer[64];
    while (1) {
        printf("%s", prompt);
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            return false;
        }
        trim_newline(buffer);
        if (strlen(buffer) == 0) {
            printf("  [Error] Input cannot be blank. Please enter a valid number.\n");
            continue;
        }
        char* endPtr;
        double val = strtod(buffer, &endPtr);
        if (*endPtr != '\0') {
            printf("  [Error] Invalid numeric value '%s'. Please enter a valid price.\n", buffer);
            continue;
        }
        *outVal = val;
        return true;
    }
}

/**
 * @brief Safely reads a line of text, trimming whitespace and newlines.
 */
void read_string(const char* prompt, char* buffer, size_t maxLen) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, (int)maxLen, stdin)) {
            trim_newline(buffer);
            /* Skip leading spaces */
            char* start = buffer;
            while (isspace((unsigned char)*start)) start++;
            if (*start == '\0') {
                printf("  [Error] Text cannot be empty. Please enter a value.\n");
                continue;
            }
            if (start != buffer) {
                memmove(buffer, start, strlen(start) + 1);
            }
            return;
        }
    }
}
