# evalhook
Stefan Esser

# How to install
## On Debian/Ubuntu

```
sudo apt-get install php5-dev build-essential git
git clone https://github.com/gmxch/evalhook
cd evalhook
phpize
./configure
make
sudo make install
```

# How to use

```
php -d extension=evalhook.so file.php
```

# INFO
## versi

num-evalhook.c [menyesuaikan versi php]

## persiapan
biar sesuai config dan php-ext
```
cd evalhook
mv num-evalhook.c evalhook.c
```
lanjut instalasi



# OPEN-ISSUE untuk request kompatibilitas phpmu
