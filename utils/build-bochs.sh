cd ..
mkdir -p .bochs
cd .bochs
wget http://garr.dl.sourceforge.net/project/bochs/bochs/2.6.2/bochs-2.6.2.tar.gz
tar -zxvf bochs-2.6.2.tar.gz
cd bochs-2.6.2
./configure --enable-smp --enable-cpu-level=6 --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx \
    --enable-debugger --enable-disasm --enable-debugger-gui --enable-logging --enable-fpu --enable-3dnow \
    --enable-sb16=dummy --enable-cdrom --enable-x86-debugger --enable-iodebug --disable-plugins --disable-docbook \
    --with-x --with-x11 --with-term
make
sudo make install
cd ../..
