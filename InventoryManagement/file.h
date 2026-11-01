/**
 * @file file.h
 * @brief Binary file handling operations for the Inventory Management System.
 *
 * Course: Data Structures and Algorithms (DSA)
 * Standard: C11
 */

#ifndef FILE_H
#define FILE_H

#include "inventory.h"

#define DEFAULT_DATA_FILE "inventory.dat"

/**
 * @brief Saves all inventory records to a binary file.
 * @param inv Pointer to the Inventory container.
 * @param filename File path to write data to.
 * @return true on success, false on I/O error.
 */
bool save_inventory_to_file(const Inventory* inv, const char* filename);

/**
 * @brief Loads inventory records from a binary file into the Inventory container.
 * Automatically handles missing files, verifies file signature, and safely recovers
 * valid records in case of file corruption.
 * @param inv Pointer to the Inventory container.
 * @param filename File path to read data from.
 * @return true if loaded (or cleanly started without file), false on fatal format error.
 */
bool load_inventory_from_file(Inventory* inv, const char* filename);

#endif /* FILE_H */
