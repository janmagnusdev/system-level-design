#!/bin/zsh
docker run -d \
  -it \
  --entrypoint bash \
  --name my-ubuntu \
  -v /Users/jan-magnus/personal-projects/system-level-design:/home/jan-magnus/system-level-design \
  my-ubuntu
docker attach my-ubuntu