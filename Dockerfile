FROM arm64v8/ubuntu

RUN apt-get update && apt-get install -y sudo
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata
RUN sudo apt install build-essential libgl1-mesa-dev cmake libxxf86vm-dev

RUN useradd -m -d /home/jan-magnus -s /bin/bash jan-magnus && echo "jan-magnus:jan-magnus" | chpasswd && usermod -a -G sudo jan-magnus

VOLUME /home/jan-magnus/system-level-design/

USER jan-magnus
WORKDIR /home/jan-magnus
CMD [ "/bin/bash" ]