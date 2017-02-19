FROM postgres:alpine

COPY ddl.sql /docker-entrypoint-initdb.d/1.sql
COPY sampleData.sql /docker-entrypoint-initdb.d/2.sql
