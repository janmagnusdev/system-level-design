#!/bin/zsh
docker container stop my-ubuntu && docker container rm my-ubuntu
docker image rm my-ubuntu