SELECT * FROM flight
JOIN named ON named.fid = flight.fid --needed for getting the human name of the flight
JOIN flightname ON flightname.prefix = named.prefix AND flightname.flightnum = named.flightnum



-- JOIN airline ON airline.prefix = flight.prefix --need prefix for flightname

-- NATURAL JOIN airport --not needed because the user provides the code
-- WHERE flight.starttime = ''
-- AND flight.endtime = ''