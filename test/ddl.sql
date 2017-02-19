CREATE TABLE Airport (
  AirportCode CHAR(3) PRIMARY KEY,
  ApName      CHAR(100),
  SAddress    CHAR(100),
  City        CHAR(100),
  Country     CHAR(100)
);

CREATE TABLE Airline (
  Prefix CHAR(3) PRIMARY KEY,
  AlName CHAR(50)
);

CREATE TABLE Plane (
  pID      SERIAL PRIMARY KEY,
  Prefix   CHAR(3) REFERENCES Airline (Prefix),
  Model    CHAR(50),
  Capacity SMALLINT,
  MaxRange SMALLINT
);


CREATE TABLE Flight (
  fID             SERIAL PRIMARY KEY,
  pID             SERIAL REFERENCES Plane (pID),
  Prefix          CHAR(3) REFERENCES Airline (Prefix),
  FromAirportCode CHAR(3) REFERENCES Airport (AirportCode),
  ToAirportCode   CHAR(3) REFERENCES Airport (AirportCode),
  StartTime       TIMESTAMP,
  EndTime         TIMESTAMP
);

CREATE TABLE FlightName (
  FlightNum SMALLINT,
  Prefix    CHAR(3) REFERENCES Airline (Prefix),
  PRIMARY KEY (FlightNum, Prefix)
);

CREATE TABLE Named (
  fID       SERIAL REFERENCES Flight (fID),
  FlightNum SMALLINT,
  Prefix    CHAR(3),
  FOREIGN KEY (FlightNum, Prefix) REFERENCES FlightName,
  PRIMARY KEY (fID, FlightNum, Prefix)
);

CREATE TABLE Client (
  GovID CHAR(100) PRIMARY KEY,
  CName CHAR(100)
);

CREATE TABLE Booking (
  bID        SERIAL PRIMARY KEY,
  DateBooked DATE,
  Cost       INT
);

CREATE TABLE Books (
  GovID CHAR(100) REFERENCES Client (GovID),
  bID   SERIAL REFERENCES Booking (bID),
  PRIMARY KEY (GovID, bID)
);

CREATE TABLE Booked (
  bID SERIAL REFERENCES Booking (bID),
  fID SERIAL REFERENCES Flight (fID),
  PRIMARY KEY (bID, fID)
);
