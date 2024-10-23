#!/bin/bash

# Running installation for all libraries
echo "Starting installation of libraries..."

# Installing SQLite
./install_sqlite.sh || { echo "SQLite installation failed"; exit 1; }

# Installing cJSON
./install_cjson.sh || { echo "cJSON installation failed"; exit 1; }

echo "All libraries have been successfully installed"
