/**
 * @file sorting.h
 * @brief Header for Merge Sort algorithm on the Product Linked List.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 *
 * Algorithm Analysis:
 *  - Time Complexity: O(N log N) in Best, Average, and Worst cases.
 *  - Space Complexity: O(log N) recursion stack, O(1) auxiliary heap memory.
 *  - Stability: Stable sorting algorithm.
 */

#ifndef SORTING_H
#define SORTING_H

#include "inventory.h"

/**
 * @brief Sorting criteria
 */
typedef enum {
    SORT_BY_NAME = 1,
    SORT_BY_PRICE,
    SORT_BY_QUANTITY
} SortCriterion;

/**
 * @brief Sorting direction
 */
typedef enum {
    ORDER_ASCENDING = 1,
    ORDER_DESCENDING
} SortOrder;

/**
 * @brief Sorts a linked list of products using the Merge Sort algorithm.
 * @param head Head pointer of the linked list to sort.
 * @param criterion Field to sort by (Name, Price, or Quantity).
 * @param order Ascending or Descending.
 * @return New head pointer of the sorted linked list.
 */
ProductNode* merge_sort(ProductNode* head, SortCriterion criterion, SortOrder order);

/**
 * @brief Interactive menu for sorting and displaying the inventory.
 * Offers choices for sorting criterion, sort order, and updates the inventory head.
 */
void inventory_sort_menu(Inventory* inv);

#endif /* SORTING_H */
