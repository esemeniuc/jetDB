FROM postgres:alpine

COPY ddl.sql /docker-entrypoint-initdb.d/1.sql
COPY sampleDataJetDB.sql /docker-entrypoint-initdb.d/2.sql

EXPOSE 5432

