# 304jetDB
An airport, flight, and booking management system built with C++ and Postgres

##Docs
####Task list
https://docs.google.com/spreadsheets/d/1o3fmZZo6sqQfygf7d-7qTpzEpZggqc4E3nXOGVpaTHg

####ER Diagram
https://drive.google.com/file/d/0B7O48vioB71peXBuZUhkb2FNOWc

####Proposal (D1)
https://docs.google.com/document/d/15pq0W9kAq_ljLixGtHVDn9wD0mrd_Ck3-KWBjZu3W1I

####ER & Schema (D2)
https://docs.google.com/document/d/1IT8hl958xbVz-hwyeYwBMfjBdKyzSMkYBtFo6epQsLk

##Usage instructions

Things you need:
- docker
- docker-compose
- a working postgres installation (we could use the docker one, but this proves you can access postgres from the local machine)

To get a working psql shell with all the data loaded, go into the test folder and run:
```
./playground.sh
```
Examine the contents for how/why that works.

##Build instructions

Make a new folder called build, and go into it.
```
cmake ..
make
```

Also, make sure your git submodules are set up:
```
git submodule update
```

Build dependencies:
- libpqxx
