## System Level Design Course SoSe2024

### Setup
For setting up this project, `sld-extern.tgz` file is required. That file includes SystemC 2.3.3. It has been provided by the Tutor of this course.

When you obtained `sld-extern.tgz` and placed it in the content root, run the following:
```bash
tar xvzf sld-extern.tgz
cd sld-extern
tar xvzf systemc-2.3.3.tar.gz
tar or1k-elf_gcc4.9.3_binutils2.26_newlib2.3.0-1_gdb7.11.tgz
make
```

This should build everything necessary, so that e.g. `assignment-20240527/adder` can be compile using `make`.

You might need to run autoconf in or1ksim, see [Build Or1ksim](#build-or1ksim).

### Information
[SLD Extern README](./sld-extern/readme.txt)

[Assignments](./assignments)  
[Slides](./slides)


### Build or1ksim

When first building or1ksim, an error is thrown. To resolve it, make sure `autotool` and `autotool-bin` are installed.  
Then, run this in or1ksim folder:
```bash
autoconf
autoreconf --install 
```

Now, continuing `make` should work just fine.