CREATE OR REPLACE FUNCTION UpdateNumTips() RETURNS trigger AS '
BEGIN 
   UPDATE  Business
   SET  numTips = numTips+1
   WHERE Business.bid = NEW.bid;
   RETURN NEW;
END
' LANGUAGE plpgsql; 

CREATE OR REPLACE FUNCTION UpdateTipCount() RETURNS trigger AS '
BEGIN 
   UPDATE  Users
   SET  tipCount = tipCount+1
   WHERE Users.u_id = NEW.u_id;
   RETURN NEW;
END
' LANGUAGE plpgsql; 

CREATE TRIGGER tipTrigger
AFTER INSERT ON Tip
FOR EACH ROW
WHEN ((NEW.bid IS NOT NULL) AND (NEW.u_id IS NOT NULL))
EXECUTE PROCEDURE UpdateNumTips();

CREATE TRIGGER tipTrigger2
AFTER INSERT ON Tip
FOR EACH ROW
WHEN ((NEW.bid IS NOT NULL) AND (NEW.u_id IS NOT NULL))
EXECUTE PROCEDURE UpdateTipCount();

CREATE OR REPLACE FUNCTION UpdateNumCheckins() RETURNS trigger AS '
BEGIN 
   UPDATE  Business
   SET  numCheckins = numCheckins+1
   WHERE Business.bid = NEW.bid;
   RETURN NEW;
END
' LANGUAGE plpgsql; 

CREATE TRIGGER checkinTrigger
AFTER INSERT ON Checkins
FOR EACH ROW
WHEN (NEW.bid IS NOT NULL)
EXECUTE PROCEDURE UpdateNumCheckins();

CREATE OR REPLACE FUNCTION UpdateLikes() RETURNS trigger AS '
BEGIN
   UPDATE  Users
   SET  totalLikes = totalLikes+1
   WHERE Users.u_id = NEW.u_id;
   RETURN NEW;
END
' LANGUAGE plpgsql; 

CREATE TRIGGER likeTrigger
AFTER UPDATE OF likes ON Tip
FOR EACH ROW
WHEN (NEW.u_id IS NOT NULL)
EXECUTE PROCEDURE UpdateLikes();