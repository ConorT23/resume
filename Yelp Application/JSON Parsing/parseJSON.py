from ast import Not
from distutils.util import execute
# from asyncio.windows_events import NULL
# from asyncio.windows_events import NULL
import json
import psycopg2
import csv
from unicodedata import name


connectionStats = "dbname='postgres' user='postgres' host='localhost' password='hi'"

def cleanStr4SQL(s):
    return s.replace("'", "`").replace("\n", " ")


def parseBusinessData():
    # read the JSON file
    # We assume that the Yelp data files are available in the current directory. If not, you should specify the path when you "open" the function. 
    with open('yelp_CptS451_2022/yelp_business.JSON', 'r') as f:
        #outfile = open('.//business.txt', 'w')
        line = f.readline()
        count_line = 0

        try:
           conn = psycopg2.connect(connectionStats)
        except:
            print('Unable to connect to the database!')
            return
        cur = conn.cursor()

        # read each JSON abject and extract data
        while line:
            data = json.loads(line)

            try:
                cur.execute("INSERT INTO business (bid, business_name, business_address, business_state, city, zipcode, latitude, longitude, stars, numCheckins, numTips, is_open)"
                       + " VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)", 
                         (data['business_id'],cleanStr4SQL(data["name"]), cleanStr4SQL(data["address"]), data["state"], data["city"], data["postal_code"], data["latitude"], data["longitude"], data["stars"], 0 , 0 , [False,True][data["is_open"]] ) )
            except Exception as e:
                print("Insert Data into Business table failed!\n",e,"\n")
                return

            # process business categories
            categories = data["categories"].split(', ')

            try:
                for cat in categories:
                    cur.execute("INSERT INTO Categories (bid, category_name)" 
                + " VALUES (%s,%s)", (cleanStr4SQL(data['business_id']), cleanStr4SQL(cat)))
            except Exception as e:
                print("Insert into Categories table failed!\n",e,"\n")
                print("Categories:\n", categories)
                return

            # outfile.write("      categories: {}\n".format(str(categories)))
            # outfile.write("      attributes:\n")

            # TO-DO : write your own code to process attributes
            # make sure to **recursively** parse all attributes at all nesting levels. You should not assume a particular nesting level.
            attrs = data['attributes']

            def parseAttributes(Bussiness_ID, attrObj):
                for k, v in attrObj.items():
                    if isinstance(v, dict):
                        try:
                            cur.execute("INSERT INTO Attributes (bid, attr_name)"
                            + " VALUES (%s, %s)", ( cleanStr4SQL(Bussiness_ID), cleanStr4SQL(k)) )
                        except Exception as e:
                            print("Insert into Attributes table failed!\n", e, '\n')
                            print("\n\n{}".format(v))
                            return
                        #outfile.write("      {}: see lowercase attributes below:\n".format(str(k)))
                        parseAttributes(Bussiness_ID, v)
                    else:
                        try:
                            cur.execute("INSERT INTO Attributes (bid, attr_name, a_value)"
                            + " VALUES (%s, %s, %s)", ( cleanStr4SQL(Bussiness_ID), cleanStr4SQL(k), cleanStr4SQL(v)))
                        except Exception as e:
                            print("Insert into Attributes table failed!\n", e, '\n')
                            print("\n**{}".format(k) +"**\n\n")
                            print("\n**{}".format(v) +"**\n\n")
                            return
                       #outfile.write("        {}; {}\n".format(str(k), str(v)))

            parseAttributes(data['business_id'], attrs)

            hours = data['hours']
            for k, v in hours.items():
                hoursplit = v.split("-")
                try:
                    cur.execute("INSERT INTO Business_Hours (bid, dayofweek, t_open, t_close)"
                        + " VALUES (%s, %s, %s, %s)", ( cleanStr4SQL(data['business_id']),cleanStr4SQL(k), cleanStr4SQL(hoursplit[0]), cleanStr4SQL(hoursplit[1])))
                except Exception as e:
                    print("Insert into Hours table failed!\n", e, '\n')
                    print("\n{}\n".format(hours.items()))
                    return

            conn.commit()
            line = f.readline()
            count_line += 1

    print(count_line)
    cur.close()
    conn.close()
    f.close()

def parseUserData():
    # TO-DO : write code to parse yelp_user.JSON
    with open('yelp_CptS451_2022/yelp_user.JSON', 'r') as f:
        #outfile = open('.//user.txt', 'w')
        line = f.readline()
        count_line = 0
        
        try:
            conn = psycopg2.connect(connectionStats)
        except:
            print('Unable to connect to the database!')
            return

        cur = conn.cursor()
        
        while line:
            data = json.loads(line)

            try:
                cur.execute("INSERT INTO Users (u_id, avg_stars, cool, fans, funny, u_name, tipCount, useful, yelping_since)"
                    + " VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)",
                    (data["user_id"], (data['average_stars']), (data['cool']), (data['fans']), 
                    (data['funny']), cleanStr4SQL(data['name']), data['tipcount'], 
                    (data['useful']), cleanStr4SQL(data['yelping_since'] ) ) )
            except Exception as e:
                print("Insert into Users table Failed!\n",e,"\n")
                return

            conn.commit()
            line = f.readline()
            count_line += 1

    print(count_line)
    cur.close()
    conn.close()
    f.close()

def parseFriendsData():

     with open('yelp_CptS451_2022/yelp_user.JSON', 'r') as f:

        line = f.readline()
        count_line = 0
        
        try:
            conn = psycopg2.connect(connectionStats)
        except:
            print('Unable to connect to the database!')
            return

        cur = conn.cursor()
        
        while line:
            data = json.loads(line)
            if isinstance(data['friends'], list):
                if data['friends'] is not None:
                    for friend in data['friends']:
                        try:
                            cur.execute("INSERT INTO friend (u_id, friend_id)"
                                + " VALUES (%s, %s)",
                                (data["user_id"], friend ) )
                        except Exception as e:
                            print("Insert into friend table Failed!\n",e,"\n")
                            print("\n\n**{}**\n\n".format(friend))
                            return
                else:
                    print("Bugging\n")
    
            conn.commit()
            line = f.readline()
            count_line += 1

        print(count_line)
        cur.close()
        conn.close()
        f.close()

def parseCheckinData():

    with open('yelp_CptS451_2022/yelp_checkin.JSON', 'r') as f:

        line = f.readline()
        count_line = 0

        try:
            conn = psycopg2.connect(connectionStats)
        except:
            print('Unable to connect to the database!')
            return
        
        cur = conn.cursor()

        while line:

            data = json.loads(line)
            dates = data['date'].split(',')

            for date in dates:

                year = date.split('-', 1)
                month = year[1].split('-', 1)
                day = month[1].split(' ', 1)
                time = day[1]

                try:
                    cur.execute("INSERT INTO Checkins (bid, c_year, c_month, c_day, c_time)"
                        + " VALUES (%s, %s, %s, %s, %s)",
                        (data["business_id"], cleanStr4SQL(year[0]), cleanStr4SQL(month[0]), cleanStr4SQL(day[0]), cleanStr4SQL(time)))
                except ValueError as v:
                    print(v,"\n")
                    cur.execute("INSERT INTO Checkins (c_year, c_month, c_day, c_time)"
                        + " VALUES (%s, %s, %s, %s)",
                        (cleanStr4SQL(year[0]), cleanStr4SQL(month[0]), cleanStr4SQL(day[0]), cleanStr4SQL(time)))
                except Exception as e:
                    print("Insert dates into Checkins failed!\n",e,"\n")
                    return

            conn.commit()
            line = f.readline()
            count_line += 1

    print(count_line)
    cur.close()
    conn.close()
    f.close()


def parseTipData():

    with open('yelp_CptS451_2022/yelp_tip.JSON', 'r') as f:

        line = f.readline()
        count_line = 0

        try:
            conn = psycopg2.connect(connectionStats)
        except:
            print('Unable to connect to the database!')
            return
        
        cur = conn.cursor()
       
        while line:
       
            data = json.loads(line)

            try:
                cur.execute("INSERT INTO Tip (bid, u_id, tipDate, likes, tipText)"
                       + " VALUES (%s, %s, %s, %s, %s)", 
                         (data["business_id"], cleanStr4SQL(data["user_id"]), cleanStr4SQL(data["date"]), data["likes"], data["text"] ) )
            
            except Exception as e:
                print("Insert to tipData failed!\n",e,"\n")
                return
            
            conn.commit()
            line = f.readline()
            count_line += 1

    print(count_line)
    cur.close()
    conn.close()
    f.close()

if __name__ == "__main__":
    parseBusinessData()
    parseUserData()
    parseFriendsData()
    parseCheckinData()
    parseTipData()
