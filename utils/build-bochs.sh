cd ..
mkdir .bochs
cd .bochs
svn co https://bochs.svn.sourceforge.net/svnroot/bochs
cd bochs
./configure --enable-smp --enable-cpu-level=6 --enable-all-optimizations --enable-x86-64 --enable-pci --enable-vmx \
    --enable-debugger --enable-disasm --enable-debugger-gui --enable-logging --enable-fpu --enable-3dnow \
    --enable-sb16=dummy --enable-cdrom --enable-x86-debugger --enable-iodebug --disable-plugins --disable-docbook \
    --with-x --with-x11 --with-term
make
sudo make install
cd ../..
rm -rf .bochs