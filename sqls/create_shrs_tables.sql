
CREATE SEQUENCE user_oid_seq;
CREATE TABLE users (
   user_oid 			int  PRIMARY KEY 
	DEFAULT nextval('user_oid_seq'), 
   user_account			CHAR(20), 
   password			CHAR(20),
   last_name			CHAR(20), 
   first_name			CHAR(20), 
   initials			CHAR(4), 
   email_address		CHAR(40), 
   is_active			bool DEFAULT true
);

CREATE TABLE system (
   system_oid		int primary key,
   comp_super_oid	int references users,
   qa_mgr_oid		int references users,
   market_mgr_oid	int references users
);

CREATE SEQUENCE seed_seq;
CREATE TABLE session (
   session_key			char(40) primary key,
   user_oid			int references users,
   expiration			timestamp
);

CREATE SEQUENCE comp_type_oid_seq;
CREATE TABLE component_types (
   comp_type_oid		int  PRIMARY KEY
	DEFAULT nextval('comp_type_oid_seq'), 
   comp_type               	CHAR(40) NOT NULL UNIQUE
);

CREATE SEQUENCE component_oid_seq;
CREATE TABLE components (
   component_oid		int  PRIMARY KEY
	DEFAULT nextval('component_oid_seq'), 
   component			CHAR(40) NOT NULL UNIQUE, 
   comp_type_oid		int DEFAULT 1 REFERENCES component_types,
   user_oid			int DEFAULT 1 REFERENCES users
);

CREATE SEQUENCE comp_rev_oid_seq;
CREATE TABLE component_revisions (
   comp_rev_oid			int  PRIMARY KEY
	DEFAULT nextval('comp_rev_oid_seq'), 
   comp_rev			CHAR(40),
   component_oid		int REFERENCES components 
);

CREATE SEQUENCE finding_act_oid_seq;
CREATE TABLE finding_activities (
   finding_act_oid		int  PRIMARY KEY
	DEFAULT nextval('finding_act_oid_seq'), 
   finding_activity		CHAR(55)  NOT NULL UNIQUE
);

CREATE SEQUENCE originator_oid_seq;
CREATE TABLE originators (
   originator_oid 		int  PRIMARY KEY 
	DEFAULT nextval('originator_oid_seq'), 
   originator			CHAR(40) NOT NULL UNIQUE
);

CREATE SEQUENCE platform_oid_seq;
CREATE TABLE platforms (
   platform_oid 		int  PRIMARY KEY 
	DEFAULT nextval('platform_oid_seq'), 
   platform			CHAR(40) NOT NULL UNIQUE
);

CREATE SEQUENCE product_oid_seq;
CREATE TABLE products (
   product_oid 			int  PRIMARY KEY 
	DEFAULT nextval('product_oid_seq'), 
   product			CHAR(40) NOT NULL UNIQUE
);

CREATE SEQUENCE pam_release_oid_seq;
CREATE TABLE pam_releases (
   pam_release_oid 		int  PRIMARY KEY 
	DEFAULT nextval('pam_release_oid_seq'), 
   pam_release			CHAR(40),
   product_oid		int REFERENCES products
);

CREATE SEQUENCE marketing_rev_oid_seq;
CREATE TABLE marketing_revisions (
   marketing_rev_oid		int  PRIMARY KEY
	DEFAULT nextval('marketing_rev_oid_seq'), 
   marketing_rev		CHAR(40),
   product_oid		int REFERENCES products 
);

CREATE SEQUENCE resolution_oid_seq;
CREATE TABLE resolutions (
   resolution_oid 		int  PRIMARY KEY 
	DEFAULT nextval('resolution_oid_seq'), 
   resolution			CHAR(40) NOT NULL UNIQUE
);

CREATE SEQUENCE request_type_oid_seq;
CREATE TABLE request_types (
   request_type_oid		int  PRIMARY KEY
	DEFAULT nextval('request_type_oid_seq'), 
   request_type               	CHAR(40) NOT NULL UNIQUE
);

CREATE SEQUENCE request_oid_seq;
CREATE TABLE requests (
   request_oid			int  PRIMARY KEY 
	DEFAULT nextval('request_oid_seq'), 
   feature_oid			int DEFAULT 1, 
   request_type_oid		int DEFAULT 1 REFERENCES request_types,
   submitter_oid		int DEFAULT 1 REFERENCES users, 
   assigned_to_oid		int DEFAULT 1 REFERENCES users, 
   verified_by_oid		int DEFAULT 1 REFERENCES users, 
   status_oid			int DEFAULT 1, 
   severity_oid			int DEFAULT 1, 
   priority_oid			int DEFAULT 1, 
   reproducibility_oid		int DEFAULT 1, 
   platform_oid			int DEFAULT 1 REFERENCES platforms, 
   product_oid		        int DEFAULT 1 REFERENCES products,
   originator_oid		int DEFAULT 1 REFERENCES originators, 
   component_oid		int DEFAULT 1 REFERENCES components, 
   comp_rev_oid			int DEFAULT 1 REFERENCES component_revisions, 
   fixed_in_comp_rev_oid	int DEFAULT 1 REFERENCES component_revisions, 
   finding_act_oid		int DEFAULT 1 REFERENCES finding_activities, 
   finding_mode_oid		int DEFAULT 1, 
   marketing_rev_oid		int DEFAULT 1 REFERENCES marketing_revisions, 
   release_date			DATE, 
   level_of_effort		CHAR(10) DEFAULT '', 
   pam_release_oid		int DEFAULT 1 REFERENCES pam_releases, 
   affects_doc_oid		int DEFAULT 1,
   date_submitted		TIMESTAMP DEFAULT now(),
   last_modified		TIMESTAMP DEFAULT now(),
   resolution_oid		int DEFAULT 1 REFERENCES resolutions, 
   resolution			TEXT, 
   title                        CHAR(80), 
   request			TEXT, 
   verification			TEXT
);

CREATE SEQUENCE status_history_oid_seq;
CREATE TABLE status_history (
   request_oid			int REFERENCES requests,
   user_oid			int REFERENCES users, 
   status			int, 
   ts				TIMESTAMP DEFAULT now()
);

CREATE SEQUENCE comment_oid_seq;
CREATE TABLE comments (
   request_oid			int REFERENCES requests,
   user_oid			int REFERENCES users, 
   comment			TEXT, 
   ts				TIMESTAMP DEFAULT now()
);


CREATE TABLE request_locks (
   request_oid		int UNIQUE NOT NULL,
   user_oid		int REFERENCES users,
   expiration		TIMESTAMP DEFAULT now()
);


