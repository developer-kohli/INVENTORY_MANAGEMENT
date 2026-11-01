/**
 * @file file.c
 * @brief Implementation of binary file persistence for the Inventory Management System.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"

#define FILE_SIGNATURE "INVM"
#define FILE_FORMAT_VERSION 1

/**
 * @brief Binary file header structure to detect format mismatches or corruption.
 */
typedef struct {
    char signature[4];        /* Magic identifier: 'I','N','V','M' */
    int version;              /* Schema version */
    int recordCount;          /* Stored record count */
} InventoryFileHeader;

/**
 * @brief Saves all inventory records in binary format.
 */
bool save_inventory_to_file(const Inventory* inv, const char* filename) {
    if (!inv || !filename) return false;

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error opening file for writing");
        return false;
    }

    /* 1. Write Header */
    InventoryFileHeader header;
    memcpy(header.signature, FILE_SIGNATURE, 4);
    header.version = FILE_FORMAT_VERSION;
    header.recordCount = inv->totalProducts;

    if (fwrite(&header, sizeof(InventoryFileHeader), 1, fp) != 1) {
        printf("  [Error] Failed to write file header to '%s'.\n", filename);
        fclose(fp);
        return false;
    }

    /* 2. Write Product Records sequentially from Linked List */
    const ProductNode* curr = inv->head;
    int writtenCount = 0;

    while (curr != NULL) {
        if (fwrite(&curr->product, sizeof(Product), 1, fp) != 1) {
            printf("  [Error] Failed to write product ID %d to file.\n", curr->product.id);
            fclose(fp);
            return false;
        }
        writtenCount++;
        curr = curr->next;
    }

    fclose(fp);
    return true;
}

/**
 * @brief Loads inventory records from binary file.
 */
bool load_inventory_from_file(Inventory* inv, const char* filename) {
    if (!inv || !filename) return false;

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        /* Missing file is expected on first launch - handle gracefully */
        printf("[Info] Data file '%s' not found. Starting with a fresh empty inventory.\n", filename);
        return true;
    }

    /* 1. Read and Validate Header */
    InventoryFileHeader header;
    if (fread(&header, sizeof(InventoryFileHeader), 1, fp) != 1) {
        printf("[Warning] Data file '%s' is empty or header is corrupted. Starting clean.\n", filename);
        fclose(fp);
        return false;
    }

    if (memcmp(header.signature, FILE_SIGNATURE, 4) != 0) {
        printf("[Error] File '%s' has an invalid binary signature! Data file may be corrupted.\n", filename);
        fclose(fp);
        return false;
    }

    if (header.version != FILE_FORMAT_VERSION) {
        printf("[Warning] File version %d differs from application version %d.\n",
               header.version, FILE_FORMAT_VERSION);
    }

    /* 2. Read Product Records */
    int loadedCount = 0;
    int corruptedCount = 0;
    char errorBuf[128];

    for (int i = 0; i < header.recordCount; i++) {
        Product temp;
        if (fread(&temp, sizeof(Product), 1, fp) != 1) {
            printf("[Warning] Unexpected end of file at record %d of %d.\n", i + 1, header.recordCount);
            break;
        }

        /* Enforce string null termination safeguards */
        temp.name[MAX_NAME_LEN - 1] = '\0';
        temp.category[MAX_CAT_LEN - 1] = '\0';

        /* Sanity validation on loaded data */
        if (temp.id <= 0 || temp.quantity < 0 || temp.price <= 0.0) {
            corruptedCount++;
            continue;
        }

        /* Insert into Linked List and Hash Table */
        if (inventory_add_product(inv, temp, errorBuf, sizeof(errorBuf))) {
            loadedCount++;
        } else {
            corruptedCount++;
        }
    }

    fclose(fp);

    printf("[System] Data load complete: %d product(s) loaded successfully.\n", loadedCount);
    if (corruptedCount > 0) {
        printf("[Warning] %d invalid/corrupted record(s) were skipped safely.\n", corruptedCount);
    }

    return true;
}
