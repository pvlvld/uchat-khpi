#!/bin/bash

# Script to install all dependencies for UChat
# This replaces the bundled libraries in ./libraries with system packages
#
# Note: This script is designed for Debian/Ubuntu systems and requires sudo access

set -e

echo "=========================================="
echo "UChat Dependency Installation Script"
echo "=========================================="
echo ""
echo "This script will install system packages using apt-get."
echo "You may be prompted for your password to use sudo."
echo ""

# Check if running with sufficient privileges for apt
if ! command -v apt-get &> /dev/null; then
    echo "Error: apt-get not found. This script is designed for Debian/Ubuntu systems."
    exit 1
fi

# Function to check if a package is installed
is_package_installed() {
    dpkg -l "$1" 2>/dev/null | grep -q "^ii"
}

# Update package list
echo "Updating package list..."
sudo apt-get update -qq

# Install build essentials if not present
echo "Checking for build essentials..."
if ! is_package_installed "build-essential"; then
    echo "Installing build-essential..."
    sudo apt-get install -y build-essential
fi

if ! is_package_installed "clang"; then
    echo "Installing clang..."
    sudo apt-get install -y clang
fi

# Install SQLite development files
echo "Checking for SQLite..."
if ! is_package_installed "libsqlite3-dev"; then
    echo "Installing SQLite development files..."
    sudo apt-get install -y libsqlite3-dev sqlite3
else
    echo "SQLite development files already installed."
fi

# Install PostgreSQL development files
echo "Checking for PostgreSQL client library..."
if ! is_package_installed "libpq-dev"; then
    echo "Installing PostgreSQL development files..."
    sudo apt-get install -y libpq-dev
else
    echo "PostgreSQL development files already installed."
fi

# Install OpenSSL development files
echo "Checking for OpenSSL..."
if ! is_package_installed "libssl-dev"; then
    echo "Installing OpenSSL development files..."
    sudo apt-get install -y libssl-dev
else
    echo "OpenSSL development files already installed."
fi

# Install cJSON development files
echo "Checking for cJSON..."
if ! is_package_installed "libcjson-dev"; then
    echo "Installing cJSON development files..."
    sudo apt-get install -y libcjson-dev libcjson1
else
    echo "cJSON development files already installed."
fi

# Install GTK3 development files (for client)
echo "Checking for GTK3..."
if ! is_package_installed "libgtk-3-dev"; then
    echo "Installing GTK3 development files..."
    sudo apt-get install -y libgtk-3-dev
else
    echo "GTK3 development files already installed."
fi

# Install pkg-config if not present
echo "Checking for pkg-config..."
if ! is_package_installed "pkg-config"; then
    echo "Installing pkg-config..."
    sudo apt-get install -y pkg-config
else
    echo "pkg-config already installed."
fi

echo ""
echo "=========================================="
echo "All dependencies installed successfully!"
echo "=========================================="
echo ""
echo "You can now build the project with:"
echo "  make all"
echo ""
