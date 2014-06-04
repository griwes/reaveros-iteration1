set -e

cd ..
mkdir -p utils/tools
mkdir -p .binutils
export PREFIX=$(pwd)/utils/tools
export TARGET=x86_64-elf
#export CC=clang
#export CXX=clang++
export CC=gcc
export CXX=g++
cd .binutils
wget http://ftp.gnu.org/gnu/binutils/binutils-2.24.tar.bz2
tar jxf binutils-2.24.tar.bz2
mv binutils-2.24 binutils
mkdir build
cd build
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-werror
make all
make install
rm -rf *
export TARGET=i686-elf
rm -rf *
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls --disable-werror
make all
make install
rm -rf *
cd ../..
rm -rf .binutils
