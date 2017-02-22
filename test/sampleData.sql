INSERT INTO Airport (AirportCode,ApName,SAddress,City,Country) VALUES ('YVR','Vancouver International Airport','3211 Grant McConachie Way','Richmond','Canada');
INSERT INTO Airport (AirportCode,ApName,SAddress,City,Country) VALUES ('JFK','John F Kennedy International Airport','NULL','Queens','USA');
INSERT INTO Airport (AirportCode,ApName,SAddress,City,Country) VALUES ('NRT','Narita International Airport','Hanedakuko, Ota','Tokyo','Japan');
INSERT INTO Airport (AirportCode,ApName,SAddress,City,Country) VALUES ('LHR','Heathrow Airport','Longford','Hounslow','UK');
INSERT INTO Airport (AirportCode,ApName,SAddress,City,Country) VALUES ('CDG','Aéroport de Paris Charles de Gaulle','95700 Roissy-en-France','Paris','France');

INSERT INTO Airline (Prefix,AlName) VALUES ('DAL','Delta Air Lines');
INSERT INTO Airline (Prefix,AlName) VALUES ('UAE','Emirates Airline');
INSERT INTO Airline (Prefix,AlName) VALUES ('CSN','China Southern Airlines');
INSERT INTO Airline (Prefix,AlName) VALUES ('DLH','Lufthansa');
INSERT INTO Airline (Prefix,AlName) VALUES ('JAL','Japan Airlines');

INSERT INTO Plane (pID,Prefix,Model,Capacity,MaxRange) VALUES ('1','DAL','Boeing 747','660','7260');
INSERT INTO Plane (pID,Prefix,Model,Capacity,MaxRange) VALUES ('2','DAL','Airbus A380','544','8500');
INSERT INTO Plane (pID,Prefix,Model,Capacity,MaxRange) VALUES ('3','UAE','Boeing 777','396','8555');
INSERT INTO Plane (pID,Prefix,Model,Capacity,MaxRange) VALUES ('4','JAL','Boeing 737','215','3000');
INSERT INTO Plane (pID,Prefix,Model,Capacity,MaxRange) VALUES ('5','DLH','Airbus A320','220','3300');

INSERT INTO Flight (fID,pID,Prefix,FromAirportCode,ToAirportCode,StartTime,EndTime) VALUES ('1','1','DAL','JFK','YVR','2016-10-15 23:39:37','2016-10-16 23:39:37');
INSERT INTO Flight (fID,pID,Prefix,FromAirportCode,ToAirportCode,StartTime,EndTime) VALUES ('2','1','DAL','YVR','JFK','2015-04-01 10:20:31','2015-04-01 14:20:31');
INSERT INTO Flight (fID,pID,Prefix,FromAirportCode,ToAirportCode,StartTime,EndTime) VALUES ('3','5','DLH','CDG','LHR','2016-11-15 15:53:49','2016-11-15 19:53:49');
INSERT INTO Flight (fID,pID,Prefix,FromAirportCode,ToAirportCode,StartTime,EndTime) VALUES ('4','5','DLH','LHR','CDG','2015-07-20 0:11:03','2015-07-20 10:11:03');
INSERT INTO Flight (fID,pID,Prefix,FromAirportCode,ToAirportCode,StartTime,EndTime) VALUES ('5','4','JAL','JFK','NRT','2015-05-23 22:04:48','2015-05-24 12:04:48');

INSERT INTO FlightName (FlightNum,Prefix) VALUES ('111','DAL');
INSERT INTO FlightName (FlightNum,Prefix) VALUES ('222','DLH');
INSERT INTO FlightName (FlightNum,Prefix) VALUES ('333','UAE');
INSERT INTO FlightName (FlightNum,Prefix) VALUES ('444','JAL');
INSERT INTO FlightName (FlightNum,Prefix) VALUES ('555','CSN');

INSERT INTO Named (fID,FlightNum,Prefix) VALUES ('1','111','DAL');
INSERT INTO Named (fID,FlightNum,Prefix) VALUES ('2','111','DAL');
INSERT INTO Named (fID,FlightNum,Prefix) VALUES ('3','222','DLH');
INSERT INTO Named (fID,FlightNum,Prefix) VALUES ('4','222','DLH');
INSERT INTO Named (fID,FlightNum,Prefix) VALUES ('5','333','UAE');
INSERT INTO Named (fID,FlightNum,Prefix) VALUES ('5','444','JAL');

INSERT INTO Client (GovID,CName) VALUES ('12345678','John Smith');
INSERT INTO Client (GovID,CName) VALUES ('22223333','Alice Doe');
INSERT INTO Client (GovID,CName) VALUES ('33334444','Jane Woodfield');
INSERT INTO Client (GovID,CName) VALUES ('44445555','Friðrik Osamu');
INSERT INTO Client (GovID,CName) VALUES ('55556666','Krystyn Gopal');

INSERT INTO Booking (bID) VALUES ('1');
INSERT INTO Booking (bID) VALUES ('2');
INSERT INTO Booking (bID) VALUES ('3');
INSERT INTO Booking (bID) VALUES ('4');
INSERT INTO Booking (bID) VALUES ('5');

INSERT INTO Books (GovID,bID,DateBooked,Cost) VALUES ('22223333','1','2016-12-31 22:05:49','544');
INSERT INTO Books (GovID,bID,DateBooked,Cost) VALUES ('44445555','2','2015-05-06 4:58:54','123');
INSERT INTO Books (GovID,bID,DateBooked,Cost) VALUES ('12345678','3','2017-01-23 4:34:09','458');
INSERT INTO Books (GovID,bID,DateBooked,Cost) VALUES ('12345678','4','2016-03-16 3:51:53','454');
INSERT INTO Books (GovID,bID,DateBooked,Cost) VALUES ('55556666','5','2016-11-14 20:40:44','989');

INSERT INTO Booked (bID,fID) VALUES ('1','1');
INSERT INTO Booked (bID,fID) VALUES ('1','2');
INSERT INTO Booked (bID,fID) VALUES ('2','5');
INSERT INTO Booked (bID,fID) VALUES ('3','4');
INSERT INTO Booked (bID,fID) VALUES ('3','3');

