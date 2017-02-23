--get flights
SELECT CONCAT(flight.prefix, flightname.flightnum) AS flightname,
airline.alname, flight.starttime, flight.endtime FROM flight
JOIN named ON named.fid = flight.fid --needed for getting the human name of the flight
JOIN flightname ON flightname.prefix = named.prefix AND flightname.flightnum = named.flightnum
JOIN airline ON airline.prefix = flight.prefix
WHERE fromairportcode = 'JFK'
AND toairportcode = 'YVR'
AND starttime > '2015-01-01 00:00:00'
AND endtime < '2017-01-01 00:00:00';

--get flights natural join
SELECT CONCAT(flight.prefix, flightname.flightnum) AS flightname,
airline.alname, flight.starttime, flight.endtime FROM flight
NATURAL JOIN named --needed for getting the human name of the flight
NATURAL JOIN flightname
NATURAL JOIN airline
WHERE fromairportcode = 'JFK'
AND toairportcode = 'YVR'
AND starttime > '2015-01-01 00:00:00'
AND endtime < '2017-01-01 00:00:00';

--get airports
-- SELECT * FROM airport

--2016-10-15 23:39:37.000000
--2016-10-16 23:39:37.000000

SELECT
  CONCAT(flight.prefix, flightname.flightnum) AS flightname,
  airline.alname,
  flight.starttime,
  flight.endtime
FROM
  flight
  NATURAL JOIN named
  NATURAL JOIN flightname
  NATURAL JOIN airline
WHERE fromairportcode = 'JFK' AND toairportcode = 'YVR' AND starttime >= '2016-10-15 23:39:37.000000' AND
      endtime <= '2016-10-16 23:39:37.000000'
