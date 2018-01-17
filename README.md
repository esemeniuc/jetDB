# 304jetDB
A client-server based flight booking and tracking system, supporting multi-hop flights, code-sharing, group bookings, and least cost path finding

### Features
- Automatically reconnects over unreliable connections with ZeroMQ message queuing
- Host-independent reproducible testing with Catch test framework and Docker containers
- Extensible JSON interface with GUI (ImGui) and CLI clients


## Docs
#### Task list
https://docs.google.com/spreadsheets/d/1o3fmZZo6sqQfygf7d-7qTpzEpZggqc4E3nXOGVpaTHg

#### ER Diagram
https://drive.google.com/file/d/0B7O48vioB71peXBuZUhkb2FNOWc

#### Proposal (D1)
https://docs.google.com/document/d/15pq0W9kAq_ljLixGtHVDn9wD0mrd_Ck3-KWBjZu3W1I

#### ER & Schema (D2)
https://docs.google.com/document/d/1IT8hl958xbVz-hwyeYwBMfjBdKyzSMkYBtFo6epQsLk

## Usage instructions

Things you need:
- python3
- docker
- docker-compose
- a working postgres installation (we could use the docker one, but this proves you can access postgres from the local machine)

### Server Launch
To get a working psql shell with all the data loaded
- set up docker (add yourself to docker group)
- go into the test folder and run:
```
./playground.sh ../build/src/server
```
Examine the contents for how/why that works.

### Client Launch:
- Set up python (for client)
```
python3 -m venv venv #make a virtual environment
source venv/bin/activate #use new virtual environment
python setup.py develop
```
- Go to src/client
```
jetdb
```

## Build instructions

Make a new folder called build, and go into it.
```
cmake ..
make
```

Also, make sure your git submodules are set up:
```
git submodule init
git submodule update
```

## Testing
From the build folder run
```
make && ctest --output-on-failure
```

Build dependencies:
- libpqxx
- zeroMQ
