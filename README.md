## System Level Design Course SoSe2024

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