/*
 * db_shrs.h
 *
 * Copyright 2002 John Atkins, Santa Barbara, California, USA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted only as authorized by John Atkins's 
 * Public License.  A copy of this license is available at
 * http://www.johnatkins.net/license.html or in file LICENSE in the
 * top-level directory of the distribution.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to John Atkins in Santa Barbara. The name of John Atkins 
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

#ifndef _DB_SHRS_H
#define _DB_SHRS_H

#include "psql.h"
#include "shrs.h"

// #include "http_config.h"
// #include "http_protocol.h"
// #include "ap_config.h"
// #include "http_log.h"
// #include "apache_request.h"


/////////////////////////////////////////////////////////////////
// Enumerations

typedef enum { 
CL_SESSION,
CL_COMPONENT,
CL_COMP_TYPE,
CL_COMP_REV,
CL_PRODUCT,
CL_PAM,
CL_MARKET_REV,
CL_FINDING_ACT,
CL_ORIG,
CL_PLATFORM,
CL_RESOLUTION,
CL_USER,
CL_USER_TYPE,
CL_NEW_REQUEST,
CL_REQUEST,
CL_REQUEST_TYPE,
CL_COMMENT,
CL_ERROR,
CL_SEVERITY,
CL_PRIORITY,
CL_FINDING_MODE,
CL_AFFECTS_DOCS,
CL_REPRODUCIBILITY,
CL_STATUS,
CL_STATUS_HISTORY,
CL_FEATURE,
CL_SUMMARIES,
CL_COMP_MGR,
CL_SYS_USERS,
CL_EMAIL_CANIDATES,
CL_USER_ACT
} CLASSES;


#define MAX_RECORD_NO		 8
// users table:
#define MAX_USER_ACCOUNT 	20
#define MAX_LAST_NAME		20 
#define MAX_FIRST_NAME		20
#define MAX_INITIALS		 4
#define MAX_PASSWORD 		20
#define MAX_EMAIL 		40
// other tables:
#define MAX_COMPONENT		40
#define MAX_COMP_TYPE		40
#define MAX_COMP_REV		40
#define MAX_FINDING_ACT		55
#define MAX_ORIGINATOR		40
#define MAX_PLATFORM		40
#define MAX_PRODUCT		40
#define MAX_PAM_RELEASE		40
#define MAX_MARKET_REV		40
#define MAX_RESOLUTION		40
#define MAX_REQUEST_TYPE	40
#define MAX_REQUEST_TITLE	80
#define MAX_LEVEL_OF_EFFORT	10

#define MAX_REQUEST		30000
#define MAX_RESOLUTION_DESC	30000
#define MAX_VERIFICATION_DESC	30000 
#define MAX_COMMENT		30000

#define MAX_DATE		10
#define MAX_TITLE		68

///////////////////////////////////////////////////////////////

char*   db_authenticates_user(database *db, char* account, char* password);

twodim* db_fetch_list(database* db, CLASSES c, char* oid);
twodim* db_fetch_status_list(database *db, user_type *ut, char* status_oid);

void*	db_fetch(database* db, CLASSES c, char* oid);

twodim* db_report(database* db, CLASSES c, char* oid, char* where);

int     db_checkout(database *db, checkout *c);
int     db_extend_checkout(database *db, checkout* c);
void    db_checkin(database *db, char* request_oid, char* user_oid);

int	db_save(database* db, CLASSES c, void* ptr);

void db_delete_all_request_locks(database *db);

void db_delete_old_sessions(database *db);
void db_delete_session(database *db, char* session_key);
int db_new_session(database *db, session* ptr);
int db_extend_session(database *db, char* session_key);
session* db_fetch_session(database *db, char* session_key);

twodim* severity_list(database* db);
static twodim* priority_list(database* db);
static twodim* finding_modes_list(database* db);
static twodim* affects_docs_list(database* db);
static twodim* reproducibility_list(database* db);
static twodim* feature_list(database* db);

static twodim* userlist_recordset(database *db);
static twodim* userreport_recordset(database *db);
static char* get_seq_sql(database* db, CLASSES c);
static char* switch_template(database* db, CLASSES c);
static user* fetch_user(database *db, char* oid);

static char* insert_session(database *db, session* ptr);
static char* insert_user(database *db, user* ptr);
static char* insert_request(database *db, new_request* ptr);
static char* insert_component(database *db, component* ptr);
static char* insert_market_rev(database *db, new_market_rev* ptr);
static char* insert_comp_rev(database *db, new_comp_rev* ptr);
static char* insert_pam_release(database *db, new_pam_release* ptr);

static char* insert_status_history(database *db, new_history* ptr);
static char* insert_comment(database *db, new_history* ptr);

static char* update_component(database *db, component* ptr);
static char* update_comp_rev(database *db, new_comp_rev* ptr);
static char* update_market_rev(database *db, new_market_rev* ptr);
static char* update_pam_release(database *db, new_pam_release* ptr);

static char* 		insert_sql(database* db, CLASSES c, void* ptr);
static char* 		update_sql(database* db, CLASSES c, void* vptr);
static new_request* 	escape_new_request(database* db, new_request* new_rec);
static new_value_key* 	escape_new_value(database* db, new_value_key* vkptr);
static component* 	escape_component(database* db, component* comp_ptr);
static new_history* 	escape_new_history(database* db, new_history* hist_ptr);
static component* 	fetch_component(database *db, char* oid);

//////////////////////////////////////////////////////////////////////////
#endif // _DB_SHRS_H
