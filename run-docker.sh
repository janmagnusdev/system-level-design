#!/bin/zsh
# the following mounts the local folder to the guest folder using a direct bind
# volumes are to be preferred, but since we just actually want to share this data directly, this solution is fine

# -e DISPLAY=docker.for.mac.host.internal:0 -> this sets the display address to the docker host.
# docker.for.mac.host.internal is a keyword used in docker networking.
# X11 addresses are described here: https://askubuntu.com/questions/432255/what-is-the-display-environment-variable

# make sure to run
# open -a XQuartz
# xhost +
# first

# TODO: Create and Start instead of Run
# TODO: Check if image is present, otherwise build, then create, start and attach
# TODO: Check if container is present, but not running, then start and attach
# TODO: Check if container is running, just attach

if docker ps | grep my-ubuntu > /dev/null; then
  docker attach my-ubuntu
else
  docker run \
    --rm \
    -d \
    -e DISPLAY=docker.for.mac.host.internal:0 \
    -it \
    --network="bridge" \
    --name my-ubuntu \
    --cpus="4" \
    -v /Users/jan-magnus/personal-projects/system-level-design:/home/sld/system-level-design \
    -v ~/.Xauthority:/root/.Xauthority \
    my-ubuntu \
    /bin/bash
  docker attach my-ubuntu
fi