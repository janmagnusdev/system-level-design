#!/bin/zsh
# the following mounts the local folder to the guest folder using a direct bind
# volumes are to be preferred, but since we just actually want to share this data directly, this solution is fine
docker run -d \
  -e DISPLAY=docker.for.mac.host.internal:0 \
  -it \
  --entrypoint bash \
  --name my-ubuntu \
  --cpus="4" \
  -v /Users/jan-magnus/personal-projects/system-level-design:/home/sld/system-level-design \
  my-ubuntu
# makes sure to attach the current shell to the containers
# TODO: maybe this can be done directly in run command, but I'm not sure how
docker attach my-ubuntu