#!/bin/bash

USER="user"
GROUP="user"

# Create installs directory
mkdir -p installs
sudo chown -R $(whoami):$(whoami) installs
sudo chmod -R u+rwx installs

# Installing SQLite
echo "Installing SQLite..."

# Change to the SQLite installation directory
cd "installs/" || { echo "Failed to change to installs directory"; exit 1; }

if [ -f "sqlite-autoconf-3460000.tar.gz" ]; then
  echo "File sqlite-autoconf-3460000.tar.gz already exists, removing..."
  sudo rm -f "sqlite-autoconf-3460000.tar.gz" || { echo "Failed to remove existing file sqlite-autoconf-3460000.tar.gz"; exit 1; }
fi

curl -O https://www.sqlite.org/2024/sqlite-autoconf-3460000.tar.gz || { echo "Failed to download sqlite-autoconf-3460000.tar.gz"; exit 1; }

if [ -d "sqlite-autoconf-3460000" ]; then
  echo "Directory sqlite-autoconf-3460000 already exists, removing..."
  sudo rm -rf sqlite-autoconf-3460000 || { echo "Failed to remove directory sqlite-autoconf-3460000"; exit 1; }
fi

# Extracting
tar -xzf sqlite-autoconf-3460000.tar.gz || { echo "Failed to extract sqlite-autoconf-3460000.tar.gz"; exit 1; }
sudo rm -f sqlite-autoconf-3460000.tar.gz || { echo "Failed to remove sqlite-autoconf-3460000.tar.gz"; exit 1; }

cd sqlite-autoconf-3460000 || { echo "Failed to change to directory sqlite-autoconf-3460000"; exit 1; }

# Configuring SQLite installation
./configure --prefix=$(pwd)/../../libraries/sqlite || { echo "SQLite configuration failed"; exit 1; }

# Building SQLite
make || { echo "SQLite build failed"; exit 1; }

# Installing SQLite
sudo make install || { echo "SQLite installation failed"; exit 1; }

cd "../"
cd "../"
echo "SQLite has been successfully installed"
