cd ..
mkdir -p utils/tools
mkdir -p .gcc
export PREFIX=$(pwd)/utils/tools
export TARGET=x86_64-elf
cd .gcc
wget http://ftp.gnu.org/gnu/binutils/binutils-2.22.tar.bz2
wget http://ftp.gnu.org/gnu/gcc/gcc-4.7.1/gcc-4.7.1.tar.bz2
tar jxf binutils-2.22.tar.bz2
tar jxf gcc-4.7.1.tar.bz2
mv binutils-2.22 binutils
mv gcc-4.7.1 gcc
mkdir build
cd build
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls
make all
make install
rm -rf *
export PATH=$PATH:$PREFIX/bin/
../gcc/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make install-gcc
make all-target-libgcc
make install-target-libgcc
export TARGET=i686-elf
rm -rf *
../binutils/configure --target=$TARGET --prefix=$PREFIX --disable-nls
make all
make install
rm -rf *
../gcc/configure --target=$TARGET --prefix=$PREFIX --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make install-gcc
make all-target-libgcc
make install-target-libgcc
cd ../..
rm -rf .gcc