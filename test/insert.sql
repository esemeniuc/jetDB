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