#!/usr/bin/env bash

set -e

docker-compose down --rmi all
docker-compose build
docker-compose up -d --force-recreate
# the server might not have started yet, so keep trying until it's up
until psql --host=localhost --username=postgres -p 9001 -l; do
  sleep 1;
done;
bash -c "$@";
docker-compose down --rmi all
