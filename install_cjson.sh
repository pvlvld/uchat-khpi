#!/bin/bash

# Building cJSON
echo "Building cJSON..."

cd "libraries/" || { echo "Failed to change to libraries directory"; exit 1; }

# Checking for cJSON repository
if [ -d "cJSON" ]; then
  echo "Repository cJSON already exists, removing..."
  sudo rm -rf cJSON || { echo "Failed to remove existing repository cJSON"; exit 1; }
fi

# Cloning cJSON repository
git clone https://github.com/DaveGamble/cJSON.git || { echo "Failed to clone repository https://github.com/DaveGamble/cJSON.git"; exit 1; }

cd "cJSON" || { echo "Failed to change to directory cJSON"; exit 1; }
mkdir "build" || { echo "Failed to create build directory"; exit 1; }
cd "build" || { echo "Failed to change to build directory"; exit 1; }
cmake .. -DCMAKE_INSTALL_PREFIX=../build || { echo "cJSON configuration failed"; exit 1; }
make || { echo "Building cJSON failed"; exit 1; }
make install || { echo "cJSON installation failed"; exit 1; }

cd "../../../"
echo "cJSON has been successfully built and installed"
