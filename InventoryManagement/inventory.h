/**
 * @file inventory.h
 * @brief Header file for Inventory Management System core data structures and operations.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_NAME_LEN 64
#define MAX_CAT_LEN 32
#define HASH_TABLE_SIZE 101       /* Prime number for uniform hash distribution */
#define LOW_STOCK_THRESHOLD 5

/**
 * @brief Structure representing an individual product record.
 * Stored as a contiguous block without internal pointers for binary serialization.
 */
typedef struct {
    int id;                       /* Unique Product Identifier (Positive integer) */
    char name[MAX_NAME_LEN];      /* Product Name */
    char category[MAX_CAT_LEN];   /* Product Category */
    int quantity;                 /* Available stock quantity (Non-negative) */
    double price;                 /* Unit price (Positive value) */
} Product;

/**
 * @brief Singly linked list node for sequential product storage.
 */
typedef struct ProductNode {
    Product product;
    struct ProductNode* next;
} ProductNode;

/**
 * @brief Hash table collision chain node (Separate Chaining).
 * Stores a direct reference to the corresponding ProductNode in the linked list.
 */
typedef struct HashNode {
    int id;
    ProductNode* pNode;           /* Pointer to the linked list node */
    struct HashNode* next;        /* Pointer to next node in collision chain */
} HashNode;

/**
 * @brief Hash table structure with fixed bucket array for O(1) average lookup.
 */
typedef struct {
    HashNode* buckets[HASH_TABLE_SIZE];
    int count;                    /* Total items indexed in hash table */
} HashTable;

/**
 * @brief High-level Inventory container managing both the Linked List and Hash Table.
 */
typedef struct {
    ProductNode* head;            /* Head of product linked list */
    HashTable* table;             /* Fast-lookup hash table */
    int totalProducts;            /* Count of unique products */
} Inventory;

/* --- Lifecycle Functions --- */
Inventory* inventory_create(void);
void inventory_free(Inventory* inv);

/* --- Hash Table Operations --- */
HashTable* hashtable_create(void);
void hashtable_free(HashTable* ht);
unsigned int hashtable_hash(int id);
bool hashtable_insert(HashTable* ht, int id, ProductNode* pNode);
ProductNode* hashtable_search(const HashTable* ht, int id);
bool hashtable_delete(HashTable* ht, int id);

/* --- Core Inventory Operations --- */
bool inventory_add_product(Inventory* inv, Product prod, char* errorMsg, size_t errorSize);
ProductNode* inventory_search_product(const Inventory* inv, int id);
bool inventory_update_product(Inventory* inv, int id, const char* name, const char* category, int quantity, double price, char* errorMsg, size_t errorSize);
bool inventory_delete_product(Inventory* inv, int id);
bool inventory_restock_product(Inventory* inv, int id, int addQty, char* errorMsg, size_t errorSize);
bool inventory_sell_product(Inventory* inv, int id, int sellQty, double* totalCost, char* errorMsg, size_t errorSize);

/* --- Display & Reporting Features --- */
void inventory_display_table(const ProductNode* head);
void inventory_display_product_card(const Product* p);
void inventory_display_low_stock(const Inventory* inv);
void inventory_display_statistics(const Inventory* inv);
void inventory_display_by_category(const Inventory* inv, const char* category);

/* --- Safe Console Input Helpers --- */
void trim_newline(char* str);
bool read_int(const char* prompt, int* outVal);
bool read_double(const char* prompt, double* outVal);
void read_string(const char* prompt, char* buffer, size_t maxLen);

#endif /* INVENTORY_H */
