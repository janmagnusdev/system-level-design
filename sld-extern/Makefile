export CFLAGS=-O3 -g3
export CXXFLAGS=-O3 -g3 -std=c++17

.PHONY: all
all: lib/libsystemc.a lib/libsim.a lib/libIrrlicht.a or1k-elf

or1k-elf: or1k-elf_gcc4.9.3_binutils2.26_newlib2.3.0-1_gdb7.11.tgz
	tar xf or1k-elf_gcc4.9.3_binutils2.26_newlib2.3.0-1_gdb7.11.tgz

or1k-elf_gcc4.9.3_binutils2.26_newlib2.3.0-1_gdb7.11.tgz:
	wget https://github.com/openrisc/newlib/releases/download/v2.3.0-1/or1k-elf_gcc4.9.3_binutils2.26_newlib2.3.0-1_gdb7.11.tgz

lib/libsim.a:
	cd or1ksim; ./build.sh

lib/libIrrlicht.a:
	cd irrlicht; ./build.sh

lib/libsystemc.a: systemc-2.3.3.tar.gz
	tar xzf $<
	cd systemc-2.3.3; cmake -DCMAKE_INSTALL_PREFIX="$$PWD/.." -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_STANDARD=17 . && make install	

systemc-2.3.3.tar.gz:
	echo Please download systemc-2.3.3.tgz from http://www.accellera.org/downloads/standards/systemc
	false

clean:
	-rm -rf bin include lib share systemc-2.3.3 or1k-elf
