@echo off
title Inventory Management System
cd /d "%~dp0InventoryManagement"
if not exist "inventory_app.exe" (
    echo [Error] inventory_app.exe not found!
    echo Please compile the project first using mingw32-make or gcc.
    pause
    exit /b 1
)
inventory_app.exe

