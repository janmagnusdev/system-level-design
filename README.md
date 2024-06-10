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
Now, you can compile `assignment-20240527/adder` using `make`.

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


# X11 forwarding on macOS and docker

The Irrlicht simulation engine requires X11 for displaying its GUI. 
Docker can forward to other X11 servers, you just need to tell it to do that.

## TL;DR

```bash
open -a XQuartz
xhost +
docker run -e DISPLAY=docker.for.mac.host.internal:0 [your-container-here]
```

## Overview

A quick guide on how to setup X11 forwarding on macOS when using docker containers requiring a DISPLAY.
Works on both Intel and M1 macs!

This guide was tested on:
- macOS Catalina 10.15.4
- docker desktop 2.2.0.5 (43884) - stable release
- XQuartz 2.7.11 (xorg-server 1.18.4)
- Macbook Pro (Intel)

and

- macOS Ventura 13.1
- docker desktop 4.15.0 (93002)
- XQuartz 2.8.4
- Mac Studio (M1)

## Step-By-Step Guide


1. Install XQuartz via brew

   `$ brew install --cask xquartz`

2. Logout and login of your Mac to activate XQuartz as default X11 server

3. Start XQuartz

   `$ open -a XQuartz`

4. Go to Security Settings and ensure that "Allow connections from network clients" is on

   ![alt XQuartz Security Stettings](https://gist.github.com/sorny/969fe55d85c9b0035b0109a31cbcb088/raw/d6eb9e8b0c20e51c46c5c9eb733b7f5e1144af4f/xquartz_preferences.png "XQuartz Security Settings")

5. Restart your Mac and start XQuartz again`

   `$ open -a XQuartz`

6. Check if XQuartz is setup and running correctly

   `$ ps aux | grep Xquartz`

7. Ensure that XQuartz is running similar to this: `/opt/X11/bin/Xquartz :0 -listen tcp`

   :0 means the display is running on display port 0.
   Important is that its not saying `–nolisten tcp` which would block any X11 forwarding to the X11 display.

8. Allow X11 forwarding via xhost

   `$ xhost +`

   This allows any client to connect. If you have security concerns you can append an IP address for a whitelist mechanism.

   Alternatively, if you want to limit X11 forwarding to local containers, you can limit clients to localhost only via

   `$ xhost +localhost`

   Be ware: You will always have to run `xhost +` after a restart of X11 as this is not a persistent setting.

9. Time to test X11 forwarding

   Pull the following docker container, set the DISPLAY env and run it...
    ```
    $ docker pull sshipway/xclock
    $ docker run -e DISPLAY=docker.for.mac.host.internal:0 sshipway/xclock
    ```
Success, good old XClock should be displayed on your screen :)

![alt XClock](https://gist.github.com/sorny/969fe55d85c9b0035b0109a31cbcb088/raw/994ce6d6e3e12c531f535563ced45a93bc88e99a/xclock.png "XClock")


## Conclusion
Your Mac is now an unsecured remote X11 server on the network, be aware of this!
Stop XQuartz and X11 if you don't need it.

If you want a Docker container or actually any unix client to use your Mac as X11 server, simply set the `DISPLAY` env variable to your ip-address and display-port.
For Docker containers, you can pass the `DISPLAY` variable via `-e DISPLAY=ip:display-port` or enter the container and set the `DISPLAY` env accordingly.

## FAQs
#### Error: Can't open display: \<ip\>:0 → what to do?
Ensure you ran `xhost +`
If error is still present, ensure XQuartz is allowing network connections. If cli-arg `–nolisten tcp` is set it wont allow any outside connections...

#### Error: No protocol specified. → what to do?
When you login through some kind of a display manager, a `MIT-MAGIC-COOKIE-1` authentication cookie is created and written to your hosts `~/.Xauthority` file. That file is read by X11 clients and the cookies available there are used to authenticate the connections.
Thanks to @LevZaplatin for pointing out a workaround:
Map your hosts `~/.Xauthority` file into your docker container via `-v ~/.Xauthority:/root/.Xauthority`
Applied to the XClock sample above:
```
$ docker run -v ~/.Xauthority:/root/.Xauthority -e DISPLAY=docker.for.mac.host.internal:0 sshipway/xclock 
```