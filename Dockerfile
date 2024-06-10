FROM arm64v8/ubuntu

RUN apt-get update && apt-get install -y sudo
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata
RUN sudo apt-get -y install build-essential libgl1-mesa-dev cmake libxxf86vm-dev
RUN sudo apt-get -y install libtool libtool-bin

RUN useradd -m -d /home/sld -s /bin/bash sld && echo "sld:sld" | chpasswd && usermod -a -G sudo sld

VOLUME /home/jan-magnus/system-level-design/

USER sld
WORKDIR /home/sld
CMD [ "/bin/bash" ]