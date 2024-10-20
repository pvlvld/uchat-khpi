#!/bin/bash

USER="user"
GROUP="user"

# Создание каталога libraries
mkdir -p libraries
mkdir -p installs
sudo chown -R $(whoami):$(whoami) libraries
sudo chmod -R u+rwx libraries
sudo chown -R $(whoami):$(whoami) installs
sudo chmod -R u+rwx installs

# Установка SQLite
echo "Установка SQLite..."

# Переход в каталог установки SQLite
cd "installs/" || { echo "Не удалось перейти в каталог installs"; exit 1; }

if [ -f "sqlite-autoconf-3460000.tar.gz" ]; then
  echo "Архив sqlite-autoconf-3460000.tar.gz уже существует, удаляем..."
  sudo rm -f "sqlite-autoconf-3460000.tar.gz" || { echo "Не удалось удалить существующий архив sqlite-autoconf-3460000.tar.gz"; exit 1; }
fi

curl -O https://www.sqlite.org/2024/sqlite-autoconf-3460000.tar.gz || { echo "Не удалось скачать openssl-3.1.0.tar.gz"; exit 1; }

if [ -d "sqlite-autoconf-3460000" ]; then
  echo "Папка sqlite-autoconf-3460000 уже существует, удаляем..."
  sudo rm -rf openssl-3.1.0 || { echo "Не удалось удалить папку sqlite-autoconf-3460000"; exit 1; }
fi

# Разархивация
tar -xzf sqlite-autoconf-3460000.tar.gz || { echo "Не удалось разархивировать sqlite-autoconf-3460000.tar.gz"; exit 1; }
sudo rm -f sqlite-autoconf-3460000.tar.gz || { echo "Не удалось удалить sqlite-autoconf-3460000.tar.gz"; exit 1; }

cd sqlite-autoconf-3460000 || { echo "Не удалось перейти в каталог sqlite-autoconf-3460000"; exit 1; }

# Конфигурация установки SQLite
./configure --prefix=$(pwd)/../../libraries/sqlite || { echo "Конфигурация SQLite не удалась"; exit 1; }

# Сборка SQLite
make || { echo "Сборка SQLite не удалась"; exit 1; }

# Установка SQLite
sudo make install || { echo "Установка SQLite не удалась"; exit 1; }

cd "../"
#if [ -d "sqlite-autoconf-3460000" ]; then
#  sudo rm -rf sqlite-autoconf-3460000 || { echo "Не удалось удалить папку sqlite-autoconf-3460000"; exit 1; }
#fi
cd "../"
echo "SQLite успешно установлен"




# Сборка dotenv-c
echo "Сборка dotenv-c..."

# Переход в каталог libraries
cd "libraries/" || { echo "Не удалось перейти в каталог libraries/"; exit 1; }

# Проверка наличия репозитория dotenv-c
if [ -d "dotenv-c" ]; then
  echo "Репозиторий dotenv-c уже существует, удаляем..."
  sudo rm -rf dotenv-c || { echo "Не удалось удалить существующий репозиторий dotenv-c"; exit 1; }
fi

# Клонирование репозитория dotenv-c
git clone https://github.com/Isty001/dotenv-c.git || { echo "Не удалось склонировать репозиторий https://github.com/Isty001/dotenv-c.git"; exit 1; }

cd "dotenv-c/" || { echo "Не удалось перейти в каталог dotenv-c"; exit 1; }

# Создание каталога build и переход в него
mkdir -p build
cd build || { echo "Не удалось создать или перейти в каталог build"; exit 1; }

# Конфигурация и сборка dotenv-c
cmake .. || { echo "Конфигурация dotenv-c не удалась"; exit 1; }
cmake --build . || { echo "Сборка dotenv-c не удалась"; exit 1; }

# Опциональная установка
sudo make install || { echo "Установка dotenv-c не удалась"; exit 1; }

cd "../../../"
echo "dotenv-c успешно собрано и установлено"



# Сборка cJSON
echo "Сборка cJSON..."

cd "libraries/" || { echo "Не удалось перейти в каталог libraries/"; exit 1; }

# Проверка наличия репозитория cJSON
if [ -d "cJSON" ]; then
  echo "Репозиторий cJSON уже существует, удаляем..."
  sudo rm -rf cJSON || { echo "Не удалось удалить существующий репозиторий cJSON"; exit 1; }
fi

# Клонирование репозитория cJSON
git clone https://github.com/DaveGamble/cJSON.git || { echo "Не удалось склонировать репозиторий https://github.com/DaveGamble/cJSON.git"; exit 1; }

cd "cJSON" || { echo "Не удалось перейти в каталог cJSON/"; exit 1; }
mkdir "build" || { echo "Не удалось создать каталог build/"; exit 1; }
cd "build" || { echo "Не удалось перейти в каталог build/"; exit 1; }
cmake .. -DCMAKE_INSTALL_PREFIX=../build || { echo "Конфигурация cJSON не удалась"; exit 1; }
make || { echo "Сборка cJSON не удалась"; exit 1; }
make install  || { echo "Установка cJSON не удалась"; exit 1; }

cd "../../../"
echo "cJSON успешно собрано и установлено"