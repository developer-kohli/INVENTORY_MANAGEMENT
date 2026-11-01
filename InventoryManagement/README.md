# Inventory Management System (C11)

A high-performance, console-based **Inventory Management System** implemented in standard C (C11) as a **Data Structures and Algorithms (DSA)** college project.

---

## 1. Project Structure

```
InventoryManagement/
├── main.c        # Interactive console menu, user input validation, program entry point
├── inventory.h   # Core data structure definitions (Product, ProductNode, HashTable, Inventory)
├── inventory.c   # Implementations of Linked List, Hash Table, and Inventory business operations
├── sorting.h     # Merge Sort declarations and complexity specifications
├── sorting.c     # Divide-and-conquer Merge Sort implementation on singly linked lists
├── file.h        # Binary file serialization and persistence declarations
├── file.c        # Binary file read/write, signature validation, and corruption recovery
└── README.md     # Documentation, complexity analysis, and usage instructions
```

### Purpose of Each File

| File | Description |
| :--- | :--- |
| [`inventory.h`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/inventory.h) | Declares the `Product`, `ProductNode`, `HashNode`, `HashTable`, and `Inventory` structures, along with function prototypes for all inventory operations, reporting functions, and safe input utilities. |
| [`inventory.c`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/inventory.c) | Implements memory allocation/deallocation, hash table separate chaining, linked list insertion/deletion, product restock/sale, table formatting, and input parsing. |
| [`sorting.h`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/sorting.h) | Declares the Merge Sort interface and sort criteria (Name, Price, Quantity) along with ordering (Ascending, Descending). |
| [`sorting.c`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/sorting.c) | Implements Merge Sort on linked lists using the slow/fast pointer (Tortoise and Hare) splitting algorithm and iterative sublist merging. |
| [`file.h`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/file.h) | Header declaring persistence functions for saving and loading data to a binary file (`inventory.dat`). |
| [`file.c`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/file.c) | Implements binary persistence (`fwrite`/`fread`), file signature validation (`INVM`), and safe error recovery against corrupted data. |
| [`main.c`](file:///d:/INVENTORY_MANAGEMENT/InventoryManagement/main.c) | Provides the main interactive terminal loop, `switch-case` dispatch, input validation, transaction receipt generation, and clean shutdown handling. |

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

### Why Merge Sort for Linked Lists?
1. **$O(N \log N)$ Guaranteed**: Unlike QuickSort (which degrades to $O(N^2)$ on unfavorable pivot selections) or BubbleSort ($O(N^2)$), Merge Sort guarantees $O(N \log N)$ running time in all scenarios.
2. **No Random Access Overhead**: Linked lists do not allow $O(1)$ random indexing (`arr[i]`), making algorithms like HeapSort or standard QuickSort inefficient. Merge Sort only accesses sequential elements.
3. **$O(1)$ Auxiliary Space**: While Merge Sort on arrays requires $O(N)$ extra memory for temporary buffers, Merge Sort on linked lists achieves sorting purely by changing pointer links (`node->next`), requiring $0$ additional heap allocations!
4. **Stability**: Preserves the relative order of items with identical sort keys.

---

## 4. Compilation & Execution

### Prerequisites
- Any standard C compiler supporting **C11** (e.g., GCC, Clang, MinGW, MSVC).

### Compiling with GCC (Recommended Flags)
Open a terminal in the `InventoryManagement` folder:

```bash
cd InventoryManagement
gcc -std=c11 -Wall -Wextra -pedantic main.c inventory.c sorting.c file.c -o inventory_app
```

### Running the Application

**On Windows:**
```cmd
inventory_app.exe
```

**On Linux / macOS:**
```bash
./inventory_app
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

### Feature Highlights
1. **Add Product**: Prevents duplicate IDs in $O(1)$, ensures positive price and non-negative quantity.
2. **Display All Products**: Clean ASCII table showing ID, Name, Category, Stock, Price, and Total Valuation. Also offers sub-options for **Category Filtering** and **Summary Statistics**.
3. **Search Product**: Instant search using Hash Table by Product ID.
4. **Update Product**: Modify product details with real-time validation.
5. **Delete Product**: Removes item from both Hash Table and Linked List with safety confirmation.
6. **Restock Product**: Increment stock quantities.
7. **Sell Product**: Decrement stock, prevent overselling, and print a formatted sale receipt.
8. **Sort Inventory**: Sort by **Name**, **Price**, or **Quantity** in **Ascending** or **Descending** order via Merge Sort.
9. **Low Stock Alert**: Instantly flags items with fewer than 5 units.
10. **Save Inventory**: Manually writes binary records to disk (`inventory.dat`).
11. **Exit**: Saves records and safely frees all allocated memory.
