FROM arm64v8/ubuntu

# arm64v8/ubuntu is a mininmal ubuntu server install, so we need to install some more required software
RUN apt-get update && apt-get install -y sudo
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata
RUN sudo apt-get -y install build-essential libgl1-mesa-dev cmake libxxf86vm-dev
RUN sudo apt-get -y install libtool libtool-bin
RUN sudo apt-get -y install xorg openbox
RUN sudo apt-get -y install libxext-dev net-tools iproute2

# this adds a user called "sld" with password "sld", and gives sudo rights
RUN useradd -m -d /home/sld -s /bin/bash sld && echo "sld:sld" | chpasswd && usermod -a -G sudo sld

USER sld

WORKDIR /home/sld
CMD [ "/bin/bash" ]