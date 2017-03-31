CREATE TABLE Airport (
  AirportCode CHAR(3) PRIMARY KEY,
  ApName      VARCHAR(100),
  SAddress    VARCHAR(100),
  City        VARCHAR(100),
  Country     VARCHAR(100)
);

CREATE TABLE Airline (
  Prefix CHAR(3) PRIMARY KEY,
  AlName VARCHAR(50)
);

CREATE TABLE Plane (
  pID      SERIAL PRIMARY KEY,
  Prefix   CHAR(3) REFERENCES Airline (Prefix),
  Model    VARCHAR(50),
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
  EndTime         TIMESTAMP,
  Cost            SMALLINT
);

CREATE TABLE FlightName (
  FlightNum SMALLINT,
  Prefix    CHAR(3) REFERENCES Airline (Prefix),
  PRIMARY KEY (FlightNum, Prefix)
);

CREATE TABLE Named (
  fID       SERIAL REFERENCES Flight (fID) NOT NULL,
  FlightNum SMALLINT,
  Prefix    CHAR(3),
  FOREIGN KEY (FlightNum, Prefix) REFERENCES FlightName,
  PRIMARY KEY (fID, FlightNum, Prefix)
);

CREATE TABLE Client (
  GovID VARCHAR(100) PRIMARY KEY,
  CName VARCHAR(100)
);

CREATE TABLE Booking (
  bID SERIAL PRIMARY KEY
);

CREATE TABLE Books (
  GovID      VARCHAR(100) REFERENCES Client (GovID) NOT NULL,
  bID        SERIAL REFERENCES Booking (bID)        NOT NULL,
  DateBooked DATE,
  Cost       INT,
  PRIMARY KEY (GovID, bID)
);

CREATE TABLE Booked (
  bID SERIAL REFERENCES Booking (bID) NOT NULL,
  fID SERIAL REFERENCES Flight (fID),
  PRIMARY KEY (bID, fID)
);

-- assuming the following ring levels:
-- 0: root
-- 1: airline/airport employee
-- 2: travel agent
-- 3: public/end user
CREATE TABLE Role (
  RingLevel INT PRIMARY KEY,
  RingDesc  VARCHAR(100)
);

CREATE TABLE LoginUser (
  Email     VARCHAR(100) PRIMARY KEY,
  Password  VARCHAR(100),
  RingLevel INT REFERENCES Role (RingLevel)
);
