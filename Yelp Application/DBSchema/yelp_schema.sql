CREATE TABLE Business( --entity
	bid char(22),
	city varchar,
	business_name varchar,
	business_address varchar,
	business_state varchar,
	zipcode INTEGER,
	latitude FLOAT,
	longitude FLOAT,
	numTips INTEGER DEFAULT 0,
	numCheckins INTEGER DEFAULT 0,
	stars FLOAT,
<<<<<<< HEAD
	is_open BOOLEAN,
=======
	is_open boolean,
>>>>>>> ca99bba8dc25b3e40f6a5f4a294932299c85ba1d
	PRIMARY KEY(bid)
);

CREATE TABLE Categories ( --not optional  multivalue of business
	bid char(22) NOT NULL,
	category_name varchar,
	PRIMARY KEY (bid, category_name),
	FOREIGN KEY (bid) REFERENCES Business(bid)
);

CREATE TABLE Business_Hours( --not optional  multivalue of business
	bid char(22) NOT NULL,
	dayofweek varchar,
	t_open varchar,
	t_close varchar,
	PRIMARY KEY (bid, dayofweek),
	FOREIGN KEY (bid) REFERENCES Business(bid)
);

CREATE TABLE Attributes ( --not optional  nested attribute of business
	bid char(22) NOT NULL,
	attr_name varchar,
	a_value varchar,
	PRIMARY KEY (bid, attr_name),
	FOREIGN KEY (bid) REFERENCES Business(bid)
);

-- CREATE TABLE GoodForMeal( --optional   nested attribute of attributes
-- 	bid char(22), 
-- 	dessert varchar,
-- 	latenight varchar,
-- 	lunch varchar,
-- 	dinner varchar,
-- 	brunch varchar,
-- 	breakfast varchar,
-- 	PRIMARY KEY (bid),
-- 	FOREIGN KEY (bid) REFERENCES attributes(bid) 
-- );

-- CREATE TABLE Ambience( -- optional   nested attribute of attributes
-- 	bid char(22), 
-- 	romantic varchar,
-- 	initimate varchar,
-- 	touristy varchar,
-- 	hipster varchar,
-- 	divey varchar,
-- 	classy varchar, 
-- 	trendy varchar,
-- 	upscale varchar,
-- 	casual varchar,
-- 	PRIMARY KEY (bid),
-- 	FOREIGN KEY (bid) REFERENCES attributes(bid) 
-- );

-- CREATE TABLE parking( --optional  nested attribute of attributes
-- 	bid char(22),
-- 	garage varchar,
-- 	street varchar,
-- 	validated varchar,
-- 	lot varchar,
-- 	valet varchar,
-- 	PRIMARY KEY (bid),
-- 	FOREIGN KEY (bid) REFERENCES attributes(bid)
-- );

CREATE TABLE Checkins( --entity
	bid char(22) NOT NULL,
	c_year INTEGER,
	c_month INTEGER,
	c_day INTEGER,
	c_time VARCHAR,
	PRIMARY KEY (bid, c_year, c_month, c_day, c_time),
	FOREIGN KEY (bid) REFERENCES Business(bid)
);

-- no longer needed
-- CREATE TABLE check_in_table( --multivalue of check_in
-- 	bid char(22) NOT NULL,
-- 	check_in_date varchar,
-- 	PRIMARY KEY (bid),
-- 	FOREIGN KEY (bid) REFERENCES check_in(bid)
-- );

CREATE TABLE Users( --entity
	u_id char(22),
	avg_stars FLOAT,
	cool INTEGER,
	fans INTEGER,
	funny INTEGER,
	useful INTEGER,
	yelping_since varchar,
	u_name varchar,
	tipCount INTEGER DEFAULT 0,
	user_latitude FLOAT,
	user_longitude FLOAT,
	totalLikes INTEGER DEFAULT 0,
	PRIMARY KEY (u_id)
);

CREATE TABLE Tip( --relation
	bid char(22),
	u_id char(22),
	tipDate varchar,
	likes INTEGER,
	tipText varchar,
	PRIMARY KEY (bid,u_id,tipDate),
	FOREIGN KEY (bid) REFERENCES Business(bid),
	FOREIGN KEY (u_id) REFERENCES Users(u_id)
);

CREATE TABLE friend( --multivalue of user
	u_id char(22) NOT NULL,
	friend_id char(22),
	PRIMARY KEY (u_id, friend_id),
	FOREIGN KEY (u_id) REFERENCES Users(u_id),
	FOREIGN KEY (friend_id) REFERENCES Users(u_id)
);



-- /*  */
