/**
 * @file sorting.c
 * @brief Implementation of Merge Sort algorithm on Product Linked List.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sorting.h"

/* Case-insensitive string comparison helper */
static int compare_strings_ci(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        int diff = tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
        if (diff != 0) return diff;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

/**
 * @brief Compares two Product records based on specified criterion and sort order.
 * @return Negative if a should precede b, Positive if b should precede a, 0 if equal.
 */
static int product_compare(const Product* a, const Product* b, SortCriterion criterion, SortOrder order) {
    int result = 0;

    switch (criterion) {
        case SORT_BY_NAME:
            result = compare_strings_ci(a->name, b->name);
            /* Tie-breaker by ID for deterministic ordering */
            if (result == 0) {
                result = a->id - b->id;
            }
            break;

        case SORT_BY_PRICE:
            if (a->price < b->price) {
                result = -1;
            } else if (a->price > b->price) {
                result = 1;
            } else {
                result = a->id - b->id;
            }
            break;

        case SORT_BY_QUANTITY:
            if (a->quantity != b->quantity) {
                result = a->quantity - b->quantity;
            } else {
                result = a->id - b->id;
            }
            break;

        default:
            result = a->id - b->id;
            break;
    }

    /* Invert result if descending */
    if (order == ORDER_DESCENDING) {
        result = -result;
    }

    return result;
}

/**
 * @brief Splits a linked list into two sublists (front and back) using the
 * slow and fast pointer (Tortoise and Hare) technique.
 * Time Complexity: O(N) where N is number of nodes in list.
 */
static void split_list(ProductNode* source, ProductNode** frontRef, ProductNode** backRef) {
    ProductNode* fast;
    ProductNode* slow;

    slow = source;
    fast = source->next;

    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    /* 'slow' is before the midpoint in the list, so split it in two at that point */
    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

/**
 * @brief Merges two already-sorted linked lists into a single sorted list.
 * Uses an iterative approach with a dummy node for O(1) auxiliary memory.
 * Time Complexity: O(N + M) where N and M are lengths of sublists.
 */
static ProductNode* merge_lists(ProductNode* a, ProductNode* b, SortCriterion criterion, SortOrder order) {
    ProductNode dummy;
    ProductNode* tail = &dummy;
    dummy.next = NULL;

    while (a != NULL && b != NULL) {
        if (product_compare(&a->product, &b->product, criterion, order) <= 0) {
            tail->next = a;
            a = a->next;
        } else {
            tail->next = b;
            b = b->next;
        }
        tail = tail->next;
    }

    /* Attach remaining nodes */
    if (a != NULL) {
        tail->next = a;
    } else {
        tail->next = b;
    }

    return dummy.next;
}

/**
 * @brief Recursive Merge Sort on a Singly Linked List.
 * Recurrence relation: T(N) = 2T(N/2) + O(N) => O(N log N).
 */
ProductNode* merge_sort(ProductNode* head, SortCriterion criterion, SortOrder order) {
    /* Base case: length 0 or 1 */
    if (head == NULL || head->next == NULL) {
        return head;
    }

    ProductNode* a = NULL;
    ProductNode* b = NULL;

    /* 1. Divide list into two halves */
    split_list(head, &a, &b);

    /* 2. Conquer: Recursively sort each sublist */
    a = merge_sort(a, criterion, order);
    b = merge_sort(b, criterion, order);

    /* 3. Combine: Merge the sorted sublists */
    return merge_lists(a, b, criterion, order);
}

/**
 * @brief Interactive sorting submenu and presentation.
 */
void inventory_sort_menu(Inventory* inv) {
    if (!inv || inv->head == NULL) {
        printf("\n[Notice] Inventory is empty. Nothing to sort.\n");
        return;
    }

    printf("\n============== SORT INVENTORY (MERGE SORT) ==============\n");
    printf("1. Sort by Product Name\n");
    printf("2. Sort by Price\n");
    printf("3. Sort by Available Quantity\n");
    printf("4. Cancel & Return\n");
    printf("----------------------------------------------------------\n");

    int choice = 0;
    if (!read_int("Select sorting field (1-4): ", &choice) || choice < 1 || choice > 4) {
        printf("Invalid choice. Returning to main menu.\n");
        return;
    }
    if (choice == 4) return;

    SortCriterion criterion = (SortCriterion)choice;

    printf("\nSelect Order:\n");
    printf("1. Ascending  (e.g., A to Z, Low to High)\n");
    printf("2. Descending (e.g., Z to A, High to Low)\n");
    int orderChoice = 1;
    if (!read_int("Select order (1-2): ", &orderChoice) || (orderChoice != 1 && orderChoice != 2)) {
        printf("Invalid choice. Defaulting to Ascending order.\n");
        orderChoice = 1;
    }
    SortOrder order = (SortOrder)orderChoice;

    /* Execute Merge Sort on Linked List */
    inv->head = merge_sort(inv->head, criterion, order);

    printf("\n[Success] Inventory successfully sorted using Merge Sort (Time: O(N log N)).\n");
    printf("Hash table references remain intact and fully synchronized.\n");

    /* Display sorted table */
    inventory_display_table(inv->head);
}
