#!/bin/zsh
# the following mounts the local folder to the guest folder using a direct bind
# volumes are to be preferred, but since we just actually want to share this data directly, this solution is fine
docker run -d \
  -it \
  --entrypoint bash \
  --name my-ubuntu \
  -v /Users/jan-magnus/personal-projects/system-level-design:/home/jan-magnus/system-level-design \
  my-ubuntu
# makes sure to attach the current shell to the containers
# TODO: maybe this can be done directly in run command, but I'm not sure how
docker attach my-ubuntu