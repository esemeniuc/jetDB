--insert new booking
-- CREATE OR REPLACE FUNCTION insertBooking (p1 integer, p2 )
-- RETURNS BOOLEAN AS $total$
--
-- declare
--   bookingID SERIAL;
--
-- BEGIN
--    SELECT count(*) into total FROM COMPANY;
--    RETURN total;
-- END;
-- $total$ LANGUAGE plpgsql;
--
-- DECLARE bookingID SERIAL;

INSERT INTO booking (bid) VALUES (DEFAULT) RETURNING bid;
--INSERT INTO booking (bid) VALUES (nextval('booking_bid_seq'::regclass)) RETURNING bid; --alterante

INSERT INTO books (govid, bid, datebooked, cost) VALUES ($1, $2, $3, $4) RETURNING books;

--get total cost of flight
SELECT SUM(Cost) FROM Flight WHERE fid IN ('1' '2' '3' );
