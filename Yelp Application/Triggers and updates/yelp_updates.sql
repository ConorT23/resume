UPDATE Business
SET numCheckins = (SELECT COUNT(Checkins.bid)
                    FROM Checkins
                    WHERE Business.bid = Checkins.bid);

UPDATE Business
SET numTips = (SELECT COUNT(Tip.bid)
                    FROM Tip
                    WHERE Business.bid = Tip.bid);

CREATE TABLE fdsa (
    uid CHAR(22) PRIMARY KEY,
    totallikes INTEGER
);

INSERT INTO fdsa
  (SELECT u_id, SUM(Tip.likes)
   FROM Tip
   GROUP BY u_id);

UPDATE Users
SET totalLikes = (SELECT totalLikes 
                FROM fdsa
                WHERE Users.u_id =  fdsa.uid);

UPDATE Users
SET totallikes = 0
WHERE totallikes IS NULL;

truncate TABLE fdsa;

CREATE TABLE fdsa2 (
    uid CHAR(22) PRIMARY KEY,
    tipcount INTEGER
);

INSERT INTO fdsa2 (
    SELECT u_id, COUNT(tip.u_id)
    FROM Tip
    GROUP BY u_id);

UPDATE Users
SET tipCount = (SELECT tipcount
                FROM fdsa2
                WHERE Users.u_id = fdsa2.uid);

UPDATE Users
SET tipCount = 0
WHERE tipCount IS NULL;

truncate TABLE fdsa2;