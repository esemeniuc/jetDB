#!/usr/bin/env bash

set -e

docker-compose build
docker-compose up -d --force-recreate
# the server might not have started yet, so keep trying until it's up
until psql --host=localhost --username=postgres; do
  continue;
done;
docker-compose down