all:
	$(MAKE) -C InventoryManagement all

test:
	$(MAKE) -C InventoryManagement test

clean:
	$(MAKE) -C InventoryManagement clean

run:
	$(MAKE) -C InventoryManagement run

.PHONY: all test clean run
