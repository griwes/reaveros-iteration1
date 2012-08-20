cd ..
mkdir -p utils/tools
mkdir -p .binutils
export PREFIX=$(pwd)/utils/tools
export TARGET=x86_64-elf
cd .binutils
wget http://ftp.gnu.org/gnu/binutils/binutils-2.22.tar.bz2
tar jxf binutils-2.22.tar.bz2
mv binutils-2.22 binutils
mkdir build
cd build
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls
make all
make install
rm -rf *
export TARGET=i686-elf
rm -rf *
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls
make all
make install
rm -rf *
cd ../..
rm -rf .binutils