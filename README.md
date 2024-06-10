## System Level Design Course SoSe2024

### Setup
For setting up this project, `sld-extern.tgz` file is required. 
That file includes SystemC 2.3.3. 
It has been provided by the Tutor of this course.

Download `sld-extern.tgz` and place it in the project root. 
Then run the following:
```bash
tar xvzf sld-extern.tgz
cd sld-extern
tar xvzf systemc-2.3.3.tar.gz
tar or1k-elf_gcc4.9.3_binutils2.26_newlib2.3.0-1_gdb7.11.tgz
make
```

This builds everything necessary. 
Compile `assignment-20240527/adder` using `make`.

You might need to run autoconf to build or1ksim, see [Build Or1ksim](#build-or1ksim).

### Information
[SLD Extern README](./sld-extern/readme.txt)

[Assignments](./assignments)  
[Slides](./slides)


### Build or1ksim

When first building or1ksim, an error might be thrown. 
To resolve it, make sure `autotool` and `autotool-bin` are installed.  
Then, run autoconf in the or1ksim folder:
```bash
cd sld-extern/or1ksim
autoconf
autoreconf --install 
```

Now, running `make` in `sld-extern` should compile everything.