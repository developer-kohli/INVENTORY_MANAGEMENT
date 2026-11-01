# Inventory Management System (C11)

A high-performance, console-based **Inventory Management System** implemented in standard C (C11) as a **Data Structures and Algorithms (DSA)** college project.

---

## 1. Project Structure

```
INVENTORY_MANAGEMENT/
├── Makefile                          # Root Makefile to build, test, and run
├── README.md                         # Project documentation and specifications
└── InventoryManagement/
    ├── main.c                        # Interactive console menu & input handling
    ├── inventory.h                   # Core structures (Product, ProductNode, HashTable, Inventory)
    ├── inventory.c                   # Linked List, Hash Table (Separate Chaining), & Operations
    ├── sorting.h                     # Merge Sort declarations & complexity analysis
    ├── sorting.c                     # Merge Sort implementation on singly linked lists
    ├── file.h                        # Binary file persistence header
    ├── file.c                        # Binary serialization, headers & corruption recovery
    ├── test_suite.c                  # Automated verification suite (10/10 tests)
    ├── Makefile                      # Module Makefile
    └── README.md                     # Module documentation
```

### Purpose of Each File

| File | Description |
| :--- | :--- |
| [`InventoryManagement/inventory.h`](InventoryManagement/inventory.h) | Declares `Product`, `ProductNode`, `HashNode`, `HashTable`, and `Inventory` structures, along with function prototypes for all inventory operations, analytics, and safe input parsing functions. |
| [`InventoryManagement/inventory.c`](InventoryManagement/inventory.c) | Implements memory allocation/deallocation, hash table separate chaining, linked list insertion/deletion, product restock/sale, table formatting, and input parsing. |
| [`InventoryManagement/sorting.h`](InventoryManagement/sorting.h) | Declares the Merge Sort interface and sort criteria (Name, Price, Quantity) along with ordering (Ascending, Descending). |
| [`InventoryManagement/sorting.c`](InventoryManagement/sorting.c) | Implements Merge Sort on linked lists using the slow/fast pointer (Tortoise and Hare) splitting algorithm and iterative sublist merging. |
| [`InventoryManagement/file.h`](InventoryManagement/file.h) | Header declaring persistence functions for saving and loading data to a binary file (`inventory.dat`). |
| [`InventoryManagement/file.c`](InventoryManagement/file.c) | Implements binary persistence (`fwrite`/`fread`), file signature validation (`INVM`), and safe error recovery against corrupted data. |
| [`InventoryManagement/main.c`](InventoryManagement/main.c) | Provides the main interactive terminal loop, `switch-case` dispatch, input validation, transaction receipt generation, and clean shutdown handling. |
| [`InventoryManagement/test_suite.c`](InventoryManagement/test_suite.c) | Automated test suite verifying 12 college project DSA requirements. |

---

## 2. Data Structures Architecture

This project integrates two primary data structures: a **Singly Linked List** and a **Hash Table with Separate Chaining**.

```
                           +-------------------------------------------------------+
                           |                     INVENTORY                         |
                           +-------------------------------------------------------+
                                   |                                       |
                   Linked List Head                                 Hash Table Buckets (Size 101)
                                   v                                       v
                     +---------------------------+              +---------------------+
                     | ProductNode               |              | Bucket [0]  -> NULL |
                     | - Product (ID: 101, etc.) | <----------------- HashNode (101)  |
                     | - next -------------------+              | Bucket [1]  -> ...  |
                                   |                            | Bucket [k]  -> ...  |
                                   v                            +---------------------+
                     +---------------------------+
                     | ProductNode               |
                     | - Product (ID: 102, etc.) | <----------------- HashNode (102)
                     | - next -------------------+
                                   |
                                  NULL
```

### 1. Singly Linked List
- **Purpose**: Dynamically stores product records in memory.
- **Why Linked List?**:
  - Unlike fixed arrays, linked lists can grow and shrink dynamically at runtime without expensive array reallocations or capacity overflows.
  - Deletions and insertions do not require shifting contiguous blocks of memory.
  - Seamlessly integrates with **Merge Sort**, which can sort linked lists in $O(N \log N)$ time without auxiliary heap memory.

### 2. Hash Table with Separate Chaining
- **Purpose**: Provides near-instantaneous $O(1)$ product lookups by **Product ID**.
- **Bucket Array**: Fixed prime size of $M = 101$ buckets (`HASH_TABLE_SIZE`). Using a prime number minimizes clustering and distributes keys uniformly.
- **Hash Function**:
  $$\text{hash}(id) = |id| \pmod{101}$$
- **Separate Chaining Collision Resolution**:
  - If two distinct product IDs hash to the same bucket index (e.g., ID 1 and ID 102 both map to bucket 1), a singly linked collision chain (`HashNode`) resolves the conflict.
  - New entries are prepended to the bucket chain in $O(1)$ time.

### 3. Synergistic Integration (Zero Data Duplication)
- The `HashNode` holds a direct pointer (`ProductNode* pNode`) to the corresponding node in the linked list.
- **Result**: Product data is stored in only one memory location (the linked list). The hash table acts as a lightweight fast-lookup index. Lookups, updates, and duplicate checks execute in $O(1)$ average time, while full inventory traversals and merge sorts operate across the linked list.

---

## 3. Algorithms & Time Complexity

| Operation | Algorithm | Best Case | Average Case | Worst Case | Space Complexity |
| :--- | :--- | :---: | :---: | :---: | :---: |
| **Search by ID** | Hash Table Lookup | $O(1)$ | $O(1)$ | $O(N)^*$ | $O(1)$ |
| **Insert Product** | Duplicate Check + Linked List Append + Hash Insert | $O(1)$ | $O(1)$ | $O(N)$ | $O(1)$ |
| **Delete Product** | Hash Table Deletion + Linked List Pointer Rewiring | $O(1)$ | $O(1)$ | $O(N)$ | $O(1)$ |
| **Sort Inventory** | Merge Sort on Linked List | $O(N \log N)$ | $O(N \log N)$ | $O(N \log N)$ | $O(\log N)$ stack |
| **Display All** | Linked List Traversal | $O(N)$ | $O(N)$ | $O(N)$ | $O(1)$ |
| **Low Stock Alert** | Sequential Scan (Filter $qty < 5$) | $O(N)$ | $O(N)$ | $O(N)$ | $O(1)$ |
| **Statistics** | Single-pass Accumulation | $O(N)$ | $O(N)$ | $O(N)$ | $O(1)$ |

\* *Worst-case occurs only if all $N$ product IDs hash into the exact same bucket.*

---

## 4. Compilation & Execution

### Option A: Using GCC Directly
```bash
cd InventoryManagement
gcc -std=c11 -Wall -Wextra -pedantic main.c inventory.c sorting.c file.c -o inventory_app
./inventory_app
```

### Option B: Using Make
```bash
mingw32-make run
```

### Running Automated Verification Tests
```bash
mingw32-make test
```

---

## 5. Console Menu Walkthrough

```
==================================================
========== INVENTORY MANAGEMENT SYSTEM ===========
==================================================
  1. Add Product
  2. Display All Products
  3. Search Product
  4. Update Product
  5. Delete Product
  6. Restock Product
  7. Sell Product
  8. Sort Inventory
  9. Low Stock Alert
 10. Save Inventory
 11. Exit
==================================================
```
