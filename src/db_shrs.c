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

#include "db_shrs.h"
#include "util.h"

#define SESSION_TIME "20 minutes"
#define CHECKOUT_TIME "20 minutes"

typedef union {
   system_users*	su;
   tag* 		t;
   new_value_key*	nv;
   new_history*		nh;
   user*		u;
   component* 		c;
   new_request*		nr;
   request_update*	r;
   summaries*		sum;
} data_ptr;

static email_canidates* fetch_email_canidates(database *db, char* oid);
static system_users* fetch_system_users(database *db);
static char* update_system_users(database *db, system_users* ptr);

static request* fetch_request(database *db, char* oid);
static summaries* fetch_summaries(database *db);
static char* format_date(ap_pool *pool, char* val);

static char* fetch_user_name(database* db, char* oid);

static void delete_old_request_locks(database *db);
static int insert_request_lock(database *db, char* request_oid, char* user_oid);
static char* fetch_user_id_of_request_lock(database *db, char* request_oid);


void db_delete_all_request_locks(database *db)
{
   execute_sql(db, "delete from request_locks;" );
}

static void delete_old_request_locks(database *db)
{
   execute_sql(db, "delete from request_locks where expiration < now();" );
}

int db_checkout(database *db, checkout *c)
{
   char* user_oid;

   if(!c) { 
      log("database checkout pointer was null");
      return 0;
   }
   c->lockedby = NULL;

   delete_old_request_locks(db);
   if(insert_request_lock(db, c->request_oid, c->user_oid))
   {
      return 1;
   } else {
    
       user_oid = fetch_user_id_of_request_lock(db, c->request_oid);
       if(user_oid) 
         c->lockedby = fetch_user_name(db, user_oid); 
       else 
         log("user id of request lock was null");
   }
   return 0;
}

void db_checkin(database *db, char* request_oid, char* user_oid)
{
   execute_sql(db, ap_pstrcat(db->pool, "delete from request_locks where request_oid=", 
		request_oid, " and user_oid=", user_oid, ";", NULL ) );
}


int db_extend_checkout(database *db, checkout *c) 
{
   char* sql;

   if(!c) { 
      log("database checkout pointer was null");
      return 0;
   }

   sql = ap_pstrcat(db->pool, 
		"UPDATE request_locks SET expiration = now()::timestamp + ('", 
		CHECKOUT_TIME, 
		"'::reltime) where request_oid=", 
		c->request_oid, 
		" and user_oid=", 
		c->user_oid, 
		";", NULL);

   return execute_sql(db, sql); 
}

/////////////////////////////////////////////////////

 static char* const states[TOTAL_STATES] = {
         "zero",
 	 "New",
 	 "Open",
 	 "Assigned",
 	 "Fixed", 
 	 "In Test",
 	 "Verified",
 	 "Request Info",
 	 "Declined",
 	 "Withdrawn",
 	 "Closed"
 };


static twodim* status_all(ap_pool *pool)
{
   int i;
   twodim *td=NULL;
   td = td_create_twodim(pool);
   td_set_dimensions(pool, td, 10, 3); 
   for(i=0;i<10;i++)
   {
      td_set_val(pool, td, i, 0, ap_psprintf(pool, "%i", i + 1));
      td_set_val(pool, td, i, 1, states[i+1]); 
   } 
   return td; 
}



static twodim* status_new(ap_pool *pool, user_type *ut )
{
   char   *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);
      

   if  ( ut->comp_mgr ) {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 5, 3); 

      // New, Open, Assigned, Request Info, Declined

      oid = ap_psprintf(pool, "%i", STATUS_NEW); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_NEW]); 

      oid = ap_psprintf(pool, "%i", STATUS_OPEN); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_OPEN]); 

      oid = ap_psprintf(pool, "%i", STATUS_ASSIGNED); 
      td_set_val(pool, td, 2, 0, oid);
      td_set_val(pool, td, 2, 1, states[STATUS_ASSIGNED ]); 

      oid = ap_psprintf(pool, "%i", STATUS_REQUEST_INFO); 
      td_set_val(pool, td, 3, 0, oid);
      td_set_val(pool, td, 3, 1, states[STATUS_REQUEST_INFO]); 

      oid = ap_psprintf(pool, "%i", STATUS_DECLINED); 
      td_set_val(pool, td, 4, 0, oid);
      td_set_val(pool, td, 4, 1, states[STATUS_DECLINED]); 
      return td;
   } 

   if ( ut->submitter ) {

      // New Withdrawn
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 2, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_NEW); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_NEW]); 

      oid = ap_psprintf(pool, "%i", STATUS_WITHDRAWN); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_WITHDRAWN]); 
      return td;
   } 

   td = td_create_twodim(pool);
   td_set_dimensions(pool, td, 1, 3); 

   oid = ap_psprintf(pool, "%i", STATUS_NEW); 
   td_set_val(pool, td, 0, 0, oid);
   td_set_val(pool, td, 0, 1, states[STATUS_NEW]); 
   return td;
}


static twodim* status_open(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->comp_mgr ) {

      // Open, Assigned, Request Info, Declined

      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 4, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_OPEN); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_OPEN]); 

      oid = ap_psprintf(pool, "%i", STATUS_ASSIGNED); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_ASSIGNED]); 

      oid = ap_psprintf(pool, "%i", STATUS_REQUEST_INFO); 
      td_set_val(pool, td, 2, 0, oid);
      td_set_val(pool, td, 2, 1, states[STATUS_REQUEST_INFO]); 

      oid = ap_psprintf(pool, "%i", STATUS_DECLINED); 
      td_set_val(pool, td, 3, 0, oid);
      td_set_val(pool, td, 3, 1, states[STATUS_DECLINED]); 

   } else {

      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_OPEN); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_OPEN]); 

   }
   return td;
}

static twodim* status_assigned(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->comp_mgr || ut->assigned_to ) {

      // Assinged, Open, Fixed

      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 3, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_ASSIGNED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_ASSIGNED]); 

      oid = ap_psprintf(pool, "%i", STATUS_OPEN); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_OPEN]); 

      oid = ap_psprintf(pool, "%i", STATUS_FIXED); 
      td_set_val(pool, td, 2, 0, oid);
      td_set_val(pool, td, 2, 1, states[STATUS_FIXED]); 

   } else {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_ASSIGNED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_ASSIGNED]); 
   }
   return td;
}

static twodim* status_fixed(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->comp_mgr ) {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 2, 3); 
   
      oid = ap_psprintf(pool, "%i", STATUS_FIXED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_FIXED]); 

      oid = ap_psprintf(pool, "%i",  STATUS_IN_TEST); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_IN_TEST]); 

   } else {

      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 
   
      oid = ap_psprintf(pool, "%i", STATUS_FIXED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_FIXED]); 
   }
   return td;
}

static twodim* status_in_test(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->verified_by ) {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 3, 3); 

      oid = ap_psprintf(pool, "%i",  STATUS_IN_TEST); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_IN_TEST]); 

      oid = ap_psprintf(pool, "%i",  STATUS_ASSIGNED); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_ASSIGNED]); 

      oid = ap_psprintf(pool, "%i",  STATUS_VERIFIED); 
      td_set_val(pool, td, 2, 0, oid);
      td_set_val(pool, td, 2, 1, states[STATUS_VERIFIED]); 

   } else {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i",  STATUS_IN_TEST); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_IN_TEST]); 

   }
   return td;
}

static twodim* status_verified(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->verified_by ) {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 2, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_VERIFIED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_VERIFIED]); 

      oid = ap_psprintf(pool, "%i", STATUS_IN_TEST); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_IN_TEST]); 

   } else {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_VERIFIED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_VERIFIED]); 
   }
   return td;
}

static twodim* status_request_info(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->submitter || ut->comp_mgr ) {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 2, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_REQUEST_INFO); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_REQUEST_INFO]); 

      oid = ap_psprintf(pool, "%i", STATUS_OPEN); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_OPEN]); 

   } else {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_REQUEST_INFO); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_REQUEST_INFO]); 
   }
   return td;
}

static twodim* status_declined(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->submitter ) {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 2, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_DECLINED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_DECLINED]); 

      oid = ap_psprintf(pool, "%i", STATUS_OPEN); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_OPEN]); 

   } else {
      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_DECLINED); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_DECLINED]); 
   }
   return td;
}

static twodim* status_withdrawn(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   if (ut->qa_mgr) return status_all(pool);

   if ( ut->submitter ) {

      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 2, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_WITHDRAWN); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_WITHDRAWN]); 

      oid = ap_psprintf(pool, "%i", STATUS_CLOSED); 
      td_set_val(pool, td, 1, 0, oid);
      td_set_val(pool, td, 1, 1, states[STATUS_CLOSED]); 

   } else {

      td = td_create_twodim(pool);
      td_set_dimensions(pool, td, 1, 3); 

      oid = ap_psprintf(pool, "%i", STATUS_WITHDRAWN); 
      td_set_val(pool, td, 0, 0, oid);
      td_set_val(pool, td, 0, 1, states[STATUS_WITHDRAWN]); 

   }
   return td;
}

static twodim* status_closed(ap_pool *pool, user_type *ut )
{
   char *oid=NULL;
   twodim *td=NULL;

   td = td_create_twodim(pool);
   td_set_dimensions(pool, td, 1, 3); 

   oid = ap_psprintf(pool, "%i", STATUS_CLOSED); 
   td_set_val(pool, td, 0, 0, oid);
   td_set_val(pool, td, 0, 1, states[STATUS_CLOSED]); 

   return td;
}

twodim* db_fetch_status_list(database *db, user_type *ut, char* status_oid)
{
   int i;

   if(!status_oid) return NULL;

   i = atoi(status_oid);
 
   switch (i) {

   case STATUS_NEW:  

      return status_new(db->pool, ut);

   case STATUS_OPEN:  

      return status_open(db->pool, ut);

   case STATUS_ASSIGNED:

      return status_assigned(db->pool, ut);

   case STATUS_FIXED:

      return status_fixed(db->pool, ut);

   case STATUS_IN_TEST:

      return status_in_test(db->pool, ut);

   case STATUS_VERIFIED:

      return status_verified(db->pool, ut);

   case STATUS_REQUEST_INFO:

      return status_request_info(db->pool, ut);

   case STATUS_DECLINED:

      return status_declined(db->pool, ut);

   case STATUS_WITHDRAWN: 

      return status_withdrawn(db->pool, ut);

   case STATUS_CLOSED: 
      return status_closed(db->pool, ut);
   }
  return NULL;
}

////////////////////////////////////////////////////

#define TOTAL_SEVERITIES	6
static char* severities[TOTAL_SEVERITIES] = {
	"Unspecified", 
	"Fatal", 
	"High", 
	"Medium", 
	"Low", 
	"Dont Know"
};

static const char* fetch_severity(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid) -  1; 
   if (i>=TOTAL_SEVERITIES || i<0) return NULL;
   return severities[i];
}

twodim* severity_list(database* db)
{
   int i;
   twodim* td;
   
   td = td_create_twodim(db->pool);
   td_set_dimensions(db->pool, td, TOTAL_SEVERITIES, 3);
   if (td)
   {
      for(i=0; i<TOTAL_SEVERITIES; i++) {
         td_set_val(db->pool, td, i, 0, ap_psprintf(db->pool, "%i", i+1) ); 
         td_set_val(db->pool, td, i, 1, severities[i]); 
      }
   }
   return td;
}

#define TOTAL_PRIORITIES	5
static const char* const priorities[TOTAL_PRIORITIES] = {
	"Unspecified", 
	"Critical", 
	"High", 
	"Medium", 
	"Low"
};

static const char* fetch_priority(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid) -  1; 
   if (i>=TOTAL_PRIORITIES || i<0) return NULL;
   return priorities[i];
}

static twodim* priority_list(database* db)
{
   int i;
   twodim* td;
   td = td_create_twodim(db->pool);
   td_set_dimensions(db->pool, td, TOTAL_PRIORITIES, 3);
   for(i=0; i<TOTAL_PRIORITIES; i++) {
      td_set_val(db->pool, td, i, 0, ap_psprintf(db->pool, "%i", i+1) ); 
      td_set_val(db->pool, td, i, 1, (char*) priorities[i]); 
   }
   return td;
}

#define TOTAL_FINDING_MODES	3
static const char* const finding_modes[TOTAL_FINDING_MODES] = {
	"Unspecified", 
	"Dynamic", 
	"Static"
};

static const char* fetch_finding_mode(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid) -  1; 
   if (i>=TOTAL_FINDING_MODES || i<0) return NULL;
   return finding_modes[i];
}

static twodim* finding_modes_list(database* db)
{
   int i;
   twodim* td;
   td = td_create_twodim(db->pool);
   td_set_dimensions(db->pool, td, TOTAL_FINDING_MODES, 3);
   for(i=0; i<TOTAL_FINDING_MODES; i++) {
      td_set_val(db->pool, td, i, 0, ap_psprintf(db->pool, "%i", i+1) ); 
      td_set_val(db->pool, td, i, 1, (char*) finding_modes[i]); 
   }
   return td;
}

#define TOTAL_AFFECTS_DOCS	4
static const char* const affects_docs[TOTAL_AFFECTS_DOCS] = {
	"Unspecified", 
	"No", 
	"Yes", 
	"Don't Know"
};

static const char* fetch_affects_doc(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid) -  1; 
   if ( i>=TOTAL_AFFECTS_DOCS || i<0 ) return NULL;
   return affects_docs[i];
}

static twodim* affects_docs_list(database* db)
{
   int i;
   twodim* td;
   td = td_create_twodim(db->pool);
   td_set_dimensions(db->pool, td, TOTAL_AFFECTS_DOCS, 3);
   for(i=0; i<TOTAL_AFFECTS_DOCS; i++) {
      td_set_val(db->pool, td, i, 0, ap_psprintf(db->pool, "%i", i+1) ); 
      td_set_val(db->pool, td, i, 1, (char*) affects_docs[i]); 
   }
   return td;
}

#define TOTAL_REPRODUCIBILITY	6
static const char* const reproducibility[TOTAL_REPRODUCIBILITY] = {
	"Unspecified", 
	"Everytime", 
	"Sometimes", 
	"Rarely", 
	"Once", 
	"New Feature"
};

static const char* fetch_reproducibility(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid) -  1; 
   if ( i>=TOTAL_REPRODUCIBILITY || i<0 ) return NULL;
   return reproducibility[i];
}

static twodim* reproducibility_list(database* db)
{
   int i;
   twodim* td;
   td = td_create_twodim(db->pool);
   td_set_dimensions(db->pool, td, TOTAL_REPRODUCIBILITY, 3);
   for(i=0; i<TOTAL_REPRODUCIBILITY; i++) {
      td_set_val(db->pool, td, i, 0, ap_psprintf(db->pool, "%i", i+1) ); 
      td_set_val(db->pool, td, i, 1, (char*) reproducibility[i]); 
   }
   return td;
}

#define TOTAL_NEW_FEATURES	3
static const char* const featutd[TOTAL_NEW_FEATURES] = {
	"Unspecified", 
	"Defect", 
	"New Feature"
};

static const char* fetch_feature(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid) -  1; 
   if ( i>=TOTAL_NEW_FEATURES || i<0 ) return NULL;
   return featutd[i];
}

static twodim* feature_list(database* db)
{
   int i;
   twodim* td;
   td = td_create_twodim(db->pool);
   td_set_dimensions(db->pool, td, TOTAL_NEW_FEATURES, 3);
   for(i=0; i<TOTAL_NEW_FEATURES; i++) {
      td_set_val(db->pool, td, i, 0, ap_psprintf(db->pool, "%i", i+1) ); 
      td_set_val(db->pool, td, i, 1, (char*) featutd[i]); 
   }
   return td;
}


static const char* fetch_status(char* oid)
{
   int i;
   if(!oid) return NULL;
   i = atoi(oid); 
   if ( i>=TOTAL_STATES || i<0 ) return NULL;
   return states[i];
}


static twodim* user_list(database *db)
{
   int i;
   char *buf;
   twodim *res;
   twodim *rtn=NULL;
   char* sql = "select user_oid, first_name, last_name from users order by last_name;";
   res = recordset(db, sql, 3); 
   if(!res) return NULL;
   rtn = td_create_twodim(db->pool); 
   td_set_dimensions(db->pool, rtn, td_row_count(res), 3);
   for(i=0;i<td_row_count(res);i++)
   {
      td_set_val(db->pool, rtn, i, 0, td_get_val(res, i, 0));
      buf = ap_pstrcat(db->pool, td_get_val(res, i, 1),
		" ", td_get_val(res, i, 2), NULL);
      td_set_val(db->pool, rtn, i, 1, buf);
   }
   return rtn; 
}

twodim* db_fetch_list(database* db, CLASSES c, char* oid)
{
   int i;
   char* buf = NULL;
   twodim *td = NULL;

   switch (c) {

   case CL_USER_ACT:
      td = recordset(db, "select user_oid, user_account from users order by user_account;", 3);
      break;

   case CL_USER:
      td = user_list(db);
      break;

   case CL_COMPONENT:
      td = recordset(db, "select component_oid, component from components order by component;", 3); 
      break;

   case CL_COMP_TYPE:
      td = recordset(db, "select * from component_types order by comp_type;", 3);      
      break;

   case CL_COMP_REV:
      if (oid) { 
         buf = ap_pstrcat(db->pool, 
		"select comp_rev_oid, comp_rev from component_revisions where component_oid=1 or component_oid =", oid, " order by comp_rev;", NULL);

         td = recordset(db, buf, 3);      
      }
      break;

   case CL_PRODUCT:
      td = recordset(db, "select * from products order by product;", 3);      

   case CL_PAM:
      if (oid) {
         buf = ap_pstrcat(db->pool, 
		"select pam_release_oid, pam_release from pam_releases where product_oid=1 or product_oid =", oid, " order by pam_release;", NULL);
         td = recordset(db, buf, 3);      
      }
      break;

   case CL_MARKET_REV:
      if (oid) {
         buf = ap_pstrcat(db->pool, 
   		"select marketing_rev_oid, marketing_rev from marketing_revisions where product_oid=1 or product_oid =", 
   		oid, " order by marketing_rev;", NULL);
         td = recordset(db, buf, 3);      
      }
      break;

   case CL_FINDING_ACT:
      td = recordset(db, "select * from finding_activities order by finding_activity;", 3);      
      break;


   case CL_ORIG:
      td = recordset(db, "select * from originators order by originator;", 3);      
      break;

   case CL_PLATFORM:
      td = recordset(db, "select * from platforms order by platform;", 3);      
      break;

   case CL_RESOLUTION:
      td = recordset(db, "select * from resolutions order by resolution;", 3);      
      break;

   case CL_REQUEST_TYPE:
      td = recordset(db, "select * from request_types order by request_type;", 3);
      break;

   case CL_SEVERITY:
      td = severity_list(db);
      break;

   case CL_PRIORITY:
      td = priority_list( db);
      break;

   case CL_FINDING_MODE:
      td = finding_modes_list( db);
      break;

   case CL_AFFECTS_DOCS:
      td = affects_docs_list( db);
      break;

   case CL_REPRODUCIBILITY:
      td = reproducibility_list( db);
      break;

   case CL_FEATURE:
      td = feature_list( db);
      break;
   
   default:
      set_error_msg(db, "Unknown class list requested");
      log("unknown class list requested");
   }
   return td;
}


// The database will return null session_user_oid or
// on null for all other user_oids.  
 
twodim* db_fetch_email_finalists(database *db, email_canidates *ptr)
{
   char* sql="SELECT email_address from users where user_oid<>";

   sql = ap_pstrcat(db->pool, sql, 
		ptr->session_user_oid, " AND (", NULL); 

   if(ptr->qa_mgr_oid)  
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->qa_mgr_oid, " OR ",  NULL);
		
   if(ptr->comp_super_oid) 
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->comp_super_oid, " OR ", NULL);

   if(ptr->comp_mgr_oid) 
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->comp_mgr_oid, " OR ", NULL);

   if(ptr->market_mgr_oid) 
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->market_mgr_oid, " OR ", NULL);

   if(ptr->submitter_oid) 
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->submitter_oid, " OR ", NULL);

   if(ptr->assigned_to_oid) 
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->assigned_to_oid, " OR ", NULL);

   if(ptr->verified_by_oid) 
	sql = ap_pstrcat(db->pool, sql, 
		"user_oid=", ptr->verified_by_oid, " OR ", NULL);

   // Remove last or.
   sql = ap_pstrndup(db->pool, sql, strlen(sql) - 4);
  
   sql = ap_pstrcat(db->pool, sql, ");", NULL); 

   return recordset(db, sql, 1); 
}

static email_canidates* fetch_email_canidates(database *db, char* oid)
{
   email_canidates  *pec=NULL;
   twodim	*td;
   char  	*sql;

   pec = (email_canidates*) ap_palloc(db->pool, sizeof(email_canidates) );
   if(!pec) return NULL;

   pec->request_oid = oid;

   td = recordset(db, "select comp_super_oid, qa_mgr_oid, 
	market_mgr_oid from system where system_oid=1;", 3);

   if(!td) return NULL;

   pec->comp_super_oid = td_get_val(td, 0, 0);
   pec->qa_mgr_oid = td_get_val(td, 0, 1);
   pec->market_mgr_oid = td_get_val(td, 0, 2);

   sql = ap_pstrcat(db->pool, "select requests.submitter_oid, requests.assigned_to_oid, requests.verified_by_oid, components.user_oid from ( requests left join components on requests.component_oid = components.component_oid ) where request_oid=", oid, ";", NULL);

   td = recordset(db, sql, 4);
   if(!td) return NULL;

   pec->submitter_oid = td_get_val(td, 0, 0);
   pec->assigned_to_oid = td_get_val(td, 0, 1);
   pec->verified_by_oid = td_get_val(td, 0, 2);
   pec->comp_mgr_oid = td_get_val(td, 0, 3);
  
   return pec; 
}

void* db_fetch(database* db, CLASSES c, char* oid )
{
   char* sql = NULL;

   if (!oid) return NULL;
 
   switch (c) {

   case CL_EMAIL_CANIDATES:
      return (email_canidates*) fetch_email_canidates(db, oid);

   case CL_SYS_USERS:
      return (system_users*) fetch_system_users(db);

   case CL_USER:
      return (user*) fetch_user(db, oid);

   case CL_COMPONENT:
      return (component*) fetch_component(db, oid);

   case CL_COMP_TYPE:
      sql = ap_pstrcat(db->pool, 
		"select comp_type from component_types", 
		" where comp_type_oid= ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_COMP_REV:
      sql = ap_pstrcat(db->pool, 
		"select comp_rev from component_revisions", 
		" where comp_rev_oid= ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_PRODUCT:
      sql = ap_pstrcat(db->pool, 
		"select product from products where product_oid = ", 
		oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_PAM:
      sql = ap_pstrcat(db->pool, 
		"select pam_release from pam_releases", 
		" where pam_release_oid= ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_MARKET_REV:
      sql = ap_pstrcat(db->pool, 
		"select marketing_rev from marketing_revisions", 
		" where marketing_rev_oid= ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_FINDING_ACT:
      sql = ap_pstrcat(db->pool, 
		"select finding_activitiy from finding_activities", 
		" where finding_act_oid = ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_ORIG:
      sql = ap_pstrcat(db->pool, "select originator from originators", 
		" where originator_oid= ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_PLATFORM:
      sql = ap_pstrcat(db->pool, "select platform from platforms", 
		" where platform_oid = ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_RESOLUTION:
      sql = ap_pstrcat(db->pool, "select resolution from resolutions", 
		" where resolution_oid = ", oid, ";", NULL);
      return (char*) fetch_str(db, sql);

   case CL_REQUEST:

      return (request*) fetch_request(db, oid);

   case CL_SUMMARIES:
      return (summaries*) fetch_summaries(db); 

   default:
      set_error_no(db, 1000);
      set_error_msg(db, "Unknown class requested"); 
      return NULL;
   }
}

static char* get_seq_sql(database* db, CLASSES c)
{
   char* buf=NULL;
 
   switch (c) {

   case CL_NEW_REQUEST:
      return "select nextval('request_oid_seq');";

   case CL_USER:
      return "select nextval('user_oid_seq');";

   case CL_COMPONENT:
      return "select nextval('component_oid_seq');";

   case CL_COMP_TYPE:
      return "select nextval('comp_type_oid_seq');";

   case CL_COMP_REV:
      return "select nextval('comp_rev_oid_seq');";

   case CL_PRODUCT:
      return "select nextval('product_oid_seq');";

   case CL_PAM:
      return "select nextval('pam_release_oid_seq');";

   case CL_MARKET_REV:
      return "select nextval('marketing_rev_oid_seq');";

   case CL_FINDING_ACT:
      return "select nextval('finding_act_oid_seq');";

   case CL_ORIG:
      return "select nextval('originator_oid_seq');";

   case CL_PLATFORM:
      return "select nextval('platform_oid_seq');";

   case CL_RESOLUTION:
      return "select nextval('resolution_oid_seq');";

   case CL_REQUEST_TYPE:
      return "select nextval('request_type_oid_seq');";

   case CL_COMMENT:
      return "select nextval('comment_oid_seq');";

   case CL_STATUS_HISTORY:
      return "select nextval('status_history_oid_seq');";

   default:
      set_error_no(db, 1000);
      set_error_msg(db, "Unknown class requested"); 
      return NULL;
   }
}

static user* fetch_user(database *db, char* oid)
{
   twodim *td;
   user* usr = NULL;
   char *sql;
   char *buf;

   if(!oid) return NULL;

   sql = ap_pstrcat(db->pool, 
	"select user_account, first_name, last_name, initials, 
	email_address, is_active from users where user_oid =", 
	oid, ";", NULL);

   td = recordset(db, sql, 6); 
   if (!td) return NULL;

   usr = (user*) ap_pcalloc(db->pool, sizeof(user) );

   if (usr)  
   {
      usr->oid		= oid;
      usr->account 	= td_get_val(td, 0, 0);
      usr->first_name 	= (db->pool, td_get_val(td, 0, 1));
      usr->last_name 	= (db->pool, td_get_val(td, 0, 2));
      usr->initials 	= (db->pool, td_get_val(td, 0, 3));
      usr->email_address = (db->pool, td_get_val(td, 0, 4));
      usr->is_active 	= strcmp("f", td_get_val(td, 0, 5));
   }
   return usr;
}


static component* fetch_component(database *db, char* oid)
{
   twodim *td;
   component* comp = NULL;
   char *sql;

   if(!oid) return NULL;

   sql = ap_pstrcat(db->pool, "select component, comp_type_oid, 
	user_oid from components where component_oid =", 
	oid, ";", NULL);

   td = recordset(db, sql, 3); 
   if(!td) return NULL;

   comp = (component*) ap_pcalloc(db->pool, sizeof(component) );
   if (comp)
   {
      comp->oid 		= oid;
      comp->name 		= td_get_val(td, 0, 0);
      comp->comp_type_oid	= td_get_val(td, 0, 1);
      comp->user_oid		= td_get_val(td, 0, 2);
   }  
   return comp;
}


/////////////////////////////////
static const char* const g_fetch_result_sql="select \
request_oid,\
requests.date_submitted,\
requests.last_modified,\
requests.release_date,\
requests.title,\
requests.feature_oid,\
requests.submitter_oid,\
requests.assigned_to_oid,\
requests.verified_by_oid,\
requests.platform_oid,\
platforms.platform,\
requests.originator_oid,\
originators.originator,\
requests.resolution_oid,\
resolutions.resolution,\
requests.request_type_oid,\
request_types.request_type,\
requests.finding_act_oid,\
finding_activities.finding_activity,\
requests.product_oid,\
products.product,\
requests.pam_release_oid,\
pam_releases.pam_release,\
requests.marketing_rev_oid,\
marketing_revisions.marketing_rev,\
requests.component_oid,\
components.component,\
component_types.comp_type,\
components.user_oid as comp_mgr_oid,\
requests.comp_rev_oid,\
component_revisions.comp_rev,\
requests.fixed_in_comp_rev_oid,\
component_revisions.comp_rev as fixedin,\
requests.status_oid,\
requests.priority_oid,\
requests.severity_oid,\
requests.reproducibility_oid,\
requests.finding_mode_oid,\
requests.affects_doc_oid,\
requests.level_of_effort,\
requests.request,\
requests.resolution as desc_resolution,\
requests.verification \
from ((((((((((( requests \
left join request_types on requests.request_type_oid= request_types.request_type_oid )\
left join platforms on requests.platform_oid  =  platforms.platform_oid )\
left join products on requests.product_oid  =  products.product_oid )\
left join originators on requests.originator_oid  =  originators.originator_oid )\
left join component_revisions on requests.comp_rev_oid =  component_revisions.comp_rev_oid \
and requests.fixed_in_comp_rev_oid = component_revisions.comp_rev_oid )\
left join finding_activities on requests.finding_act_oid =  finding_activities.finding_act_oid )\
left join marketing_revisions on requests.marketing_rev_oid =  marketing_revisions.marketing_rev_oid )\
left join pam_releases on requests.pam_release_oid =  pam_releases.pam_release_oid )\
left join resolutions on requests.resolution_oid = resolutions.resolution_oid) \
left join components on requests.component_oid  =  components.component_oid )\
right join component_types on components.comp_type_oid = component_types.comp_type_oid ) \
where request_oid=";


static char* fetch_user_name(database* db, char* oid)
{
   twodim *td;
   char* sql=NULL;

   if(!oid) return NULL;

   sql = ap_pstrcat(db->pool, 
	"select first_name, last_name from users where user_oid=", 
	oid, ";", NULL); 

   td = recordset(db, sql, 2); 
   if (!td) return NULL;

   return ap_pstrcat(db->pool, td_get_val(td, 0, 0), 
		" ", td_get_val(td, 0, 1), NULL );
}

static entries* fetch_comments(database* db, char* oid)
{
   int i;
   twodim *td; 
   char* sql;
   entries* p1=NULL;
   single_entry*  p2;

   if(!oid) return NULL;

   sql = ap_pstrcat(db->pool, "select comment, users.last_name, ts from ( comments left join users on comments.user_oid = users.user_oid ) where comments.request_oid=", oid, " order by ts;", NULL);

   td = recordset(db, sql, 3);
   if (!td) return NULL;

   if (td_row_count(td) == 0) return NULL;

   p1 = (entries*) ap_palloc(db->pool, sizeof(entries) );
   p1->pp = (single_entry**) ap_palloc(db->pool, 
		    sizeof(single_entry*)*td_row_count(td) );

   if (p1 && p1->pp) 
   {
      for(i=0; i<td_row_count(td); i++)
      {
         p2 = (single_entry*) ap_palloc(db->pool, sizeof(single_entry) );
         if(p2)
         {
            p2->text = td_get_val(td, i, 0);
            p2->user_name = td_get_val(td, i, 1);
            p2->entrydate = td_get_val(td, i, 2);
            p1->pp[i] = p2;
            p1->size = i + 1;
         }
      }
   }
   return p1;   
}

static entries* fetch_status_history(database *db, char* oid)
{
   int i;
   char* sql;
   twodim *td;
   entries* p1=NULL;
   single_entry*   p2;

   if(!oid) NULL; 

   sql = ap_pstrcat(db->pool, "select status, users.last_name, ts from ( status_history left join users on status_history.user_oid = users.user_oid ) where request_oid=", oid, " order by ts;", NULL);

   td = recordset(db, sql, 3); 
   if (!td) return NULL;

   if (td_row_count(td)>0) 
   {
      p1 = (entries*) ap_palloc(db->pool, sizeof(entries) );
      p1->pp = (single_entry**) ap_palloc(db->pool, 
		sizeof(single_entry*)*td_row_count(td));

      if (p1 && p1->pp)
      {
         for(i=0; i<td_row_count(td); i++)
         {
            p2 = (single_entry*) ap_palloc(db->pool, sizeof(single_entry) );
            if(p2)
            {
               p2->text = fetch_status(td_get_val(td, i, 0) );
               p2->user_name = td_get_val(td, i, 1);
               p2->entrydate = td_get_val(td, i, 2);
               p1->pp[i] = p2;
               p1->size = i + 1;
            }
         }
      }
   }
   return p1;   
}

const char* const g_summaries_sql="select \
requests.request_oid,\
requests.status_oid,\
requests.severity_oid,\
requests.priority_oid,\
requests.assigned_to_oid,\
components.component,\
component_revisions.comp_rev,\
requests.title,\
requests.request \
from ( requests \
left join components on components.component_oid = requests.component_oid) \
left join component_revisions on component_revisions.comp_rev_oid = requests.comp_rev_oid \
order by request_oid desc;";

static summaries* fetch_summaries(database *db)
{
   int i;
   twodim *td;
   summaries* p1=NULL;
   summary*   p2;

   log(g_summaries_sql);
   td = recordset(db, (char*) g_summaries_sql, 9); 
   if(!td) return NULL;

   if (td_row_count(td) == 0) return NULL;

   p1  = (summaries*) ap_palloc(db->pool, sizeof(summaries) );
   if (!p1) return NULL;

   p1->pp = (summary**) ap_palloc(db->pool, sizeof(summary*)*td_row_count(td));

   if (!p1->pp) return NULL; 
         
   for(i=0; i< td_row_count(td); i++)
   {
     p2 = (summary*) ap_palloc(db->pool, sizeof(summary) );
     if(p2)
     {
        p2->oid = ap_pstrdup(db->pool, td_get_val(td, i, 0));
        p2->status = fetch_status(td_get_val(td, i, 1) );
        p2->severity = fetch_severity(td_get_val(td, i, 2) );
        p2->priority = fetch_priority(td_get_val(td, i, 3) );
        p2->assigned_to = (char*) fetch_user_name(db,  td_get_val(td, i, 4) );
        p2->component = ap_pstrdup(db->pool, td_get_val(td, i, 5) );
        p2->revision = ap_pstrdup(db->pool,  td_get_val(td, i, 6) );
        p2->title = ap_pstrdup(db->pool,  td_get_val(td, i, 7) );
        p2->request = ap_pstrdup(db->pool,  td_get_val(td, i, 8) );
        p1->pp[i] = p2;
        p1->size = i + 1;
      }
   }
   return p1;   
}

static request* fetch_request(database *db, char* oid)
{
   twodim *td;
   request* ptr = NULL;
   char* sql=NULL;

   if(!oid) {
      log("NULL request");      
      return NULL;
   }

   sql = ap_pstrcat(db->pool, g_fetch_result_sql, oid, ";", NULL); 

   td = recordset(db, sql, 42); 
   if(!td) return NULL;

   ptr = (request*) ap_pcalloc(db->pool, sizeof(request) );
   if(!ptr) return NULL;

   ptr->oid = ap_pstrdup(db->pool, oid);
   ptr->date_submitted 	= format_date(db->pool, td_get_val(td, 0, 1));
   ptr->date_last_modified = format_date(db->pool, td_get_val(td, 0, 2));
   ptr->release_date 	= format_date(db->pool, td_get_val(td, 0, 3));
   ptr->title 		= trim(db->pool, td_get_val(td, 0, 4));
   ptr->feature.oid 	= td_get_val(td, 0, 5);
   ptr->feature.value 	= (char*) fetch_feature(ptr->feature.oid); 
   ptr->submitter.oid 	= td_get_val(td, 0, 6);
   ptr->submitter.value	= fetch_user_name(db, ptr->submitter.oid); 
   ptr->assigned_to.oid	= td_get_val(td, 0, 7);
   ptr->assigned_to.value = fetch_user_name(db, ptr->assigned_to.oid); 
   ptr->verified_by.oid = td_get_val(td, 0, 8); 
   ptr->verified_by.value = fetch_user_name(db, ptr->verified_by.oid); 
   ptr->platform.oid 	= td_get_val(td, 0, 9);
   ptr->platform.value 	= td_get_val(td, 0, 10);
   ptr->originator.oid 	= td_get_val(td, 0, 11);
   ptr->originator.value = td_get_val(td, 0, 12);
   ptr->resolution.oid	= td_get_val(td, 0, 13);
   ptr->resolution.value = td_get_val(td, 0, 14);
   ptr->request_type.oid = td_get_val(td, 0, 15);
   ptr->request_type.value 	= td_get_val(td, 0, 16);
   ptr->finding_activity.oid 	= td_get_val(td, 0, 17);
   ptr->finding_activity.value 	= td_get_val(td, 0, 18);
   ptr->product.oid 	= td_get_val(td, 0, 19);
   ptr->product.value 	= td_get_val(td, 0, 20);
   ptr->pam_release.oid = td_get_val(td, 0, 21);
   ptr->pam_release.value = td_get_val(td, 0, 22);
   ptr->market_rev.oid 	= td_get_val(td, 0, 23);
   ptr->market_rev.value = td_get_val(td, 0, 24);
   ptr->component.oid 	= td_get_val(td, 0, 25);
   ptr->component.value = td_get_val(td, 0, 26);
   ptr->comp_type 	= td_get_val(td, 0, 27); 
   ptr->comp_mgr.oid 	= td_get_val(td, 0, 28);
   ptr->comp_mgr.value  = fetch_user_name(db, ptr->comp_mgr.oid); 
   ptr->comp_rev.oid 	= td_get_val(td, 0, 29);
   ptr->comp_rev.value 	= td_get_val(td, 0, 30);
   ptr->fixed_in_comp_rev.oid 	= td_get_val(td, 0, 31);
   ptr->fixed_in_comp_rev.value = td_get_val(td, 0, 32);
   ptr->status.oid 	= td_get_val(td, 0, 33);
   ptr->status.value 	= (char*) fetch_status(ptr->status.oid);
   ptr->priority.oid 	= td_get_val(td, 0, 34);
   ptr->priority.value 	= (char*) fetch_priority(ptr->priority.oid); 
   ptr->severity.oid 	= td_get_val(td, 0, 35);
   ptr->severity.value 	= (char*) fetch_severity(ptr->severity.oid);
   ptr->reproducibility.oid = td_get_val(td, 0, 36);
   ptr->reproducibility.value 	= (char*) fetch_reproducibility(ptr->reproducibility.oid);
   ptr->finding_mode.oid = td_get_val(td, 0, 37);
   ptr->finding_mode.value = (char*) fetch_finding_mode(ptr->finding_mode.oid); 
   ptr->affects_docs.oid = td_get_val(td, 0, 38);
   ptr->affects_docs.value = (char*) fetch_affects_doc(ptr->affects_docs.oid); 
   ptr->level_of_effort	= td_get_val(td, 0, 39);
   ptr->desc_request	= td_get_val(td, 0, 40);
   ptr->desc_resolution 	= td_get_val(td, 0, 41);
   ptr->desc_verification = td_get_val(td, 0, 42);
   ptr->status_history	= fetch_status_history(db, oid); 
   ptr->notes		= fetch_comments(db, oid); 
   return ptr;
}


void db_delete_old_sessions(database *db)
{
   execute_sql(db, "delete from session where expiration < now();" );
}

void db_delete_session(database *db, char* session_key)
{
   execute_sql(db, ap_pstrcat(db->pool, "delete from session where session_key='", session_key, "';", NULL ));
}

static char* create_new_key(database *db )
{
   char *buf=NULL;
   int seed=1357;
   int rnd;

   buf = fetch_str(db, "select nextval('seed_seq');");
   if(buf) seed=atoi(buf);
   if(seed>32000) 
   buf = fetch_str(db, "select setval('seed_seq', 1);");

   srand(seed);
   buf = ap_pstrndup(db->pool, (char*) ap_md5(db->pool, 
		ap_psprintf(db->pool, "Sx%i3", rand())), 40);

   return buf; 
}

int db_new_session(database *db, session* ptr)
{
   char* sql;
   ptr->key = create_new_key(db);
   sql = ap_pstrcat( db->pool, "INSERT INTO session ( session_key, user_oid, expiration ) VALUES ('", 
	ptr->key, "',", ptr->user_oid, ", now()::timestamp + ('", 
	SESSION_TIME, 
	"'::reltime));", NULL);
   return execute_sql(db, sql); 
}

session* db_fetch_session(database *db, char* session_key)
{
   twodim *td;
   session *ptr=NULL;
   char* sql=NULL;

   if(!session_key) return NULL;

   sql = ap_pstrcat(db->pool, 
	"select user_oid from session where session_key='", 
	session_key, "';", NULL); 

   td = recordset(db, sql, 1); 
   if(!td) return NULL;

   ptr = (session*) ap_palloc(db->pool, sizeof(session) );

   if(!ptr) return NULL;

   ptr->key = ap_pstrdup(db->pool, session_key);
   ptr->user_oid = td_get_val(td, 0, 0);
   
   return ptr;   
}

int db_extend_session(database *db, char* session_key)
{
   char* sql;

   sql = ap_pstrcat( db->pool, 
	"UPDATE session set expiration= now()::timestamp + ('", 
	SESSION_TIME, 
	"'::reltime) where session_key='",  
	session_key,  
	"';", NULL);

   return execute_sql(db, sql); 
}

static int insert_request_lock(database *db, char* request_oid, char* user_oid)
{
   char* sql;
   sql = ap_pstrcat( db->pool, 
	"INSERT INTO request_locks ( request_oid, user_oid, expiration ) VALUES ( ", 
	request_oid, ",", user_oid, ", now()::timestamp + ('", 
	CHECKOUT_TIME, 
	"'::reltime));", NULL);

   return execute_sql(db, sql); 
}

static char* fetch_user_id_of_request_lock(database *db, char* request_oid)
{
   twodim *td;
   char* sql=NULL;

   if(!request_oid) return NULL;

   sql = ap_pstrcat(db->pool, 
	"select user_oid from request_locks where request_oid=", 
	request_oid, ";", NULL); 

   td = recordset(db, sql, 1); 
   if (!td) return NULL;

   return td_get_val(td, 0, 0); 
}


static char* insert_user(database *db, user* ptr )
{
   char* is_active	="false";

   if ( ptr->is_active ) is_active="true";

   return  ap_pstrcat( db->pool, "INSERT INTO users (", 
		"user_oid, ",
		"user_account, ",
		"password, ",
		"last_name, ",
		"first_name, ",
		"initials, ",
		"email_address,",
		"is_active ",
		") VALUES (",  
		ptr->oid, ",'",
		ptr->account, "','",
		ptr->password, "','",
		ptr->last_name, "','",
		ptr->first_name,  "','",
		ptr->initials,  "','",
		ptr->email_address, "',",
		is_active,
		");", NULL);
}


static char* update_user(database *db, user* ptr)
{
   int   len;
   char* buf= "UPDATE users SET ";

   if (!ptr) return NULL;
   if (!ptr->oid) return NULL;

   if ( ptr->account )
      buf = ap_pstrcat(db->pool, buf, "user_account='", ptr->account, "',", NULL );
   if ( ptr->password )
      buf = ap_pstrcat(db->pool, buf, "password='", ptr->password, "',", NULL );
   if ( ptr->last_name )
      buf = ap_pstrcat(db->pool, buf, "last_name='", ptr->last_name, "',", NULL );
   if ( ptr->first_name )
      buf = ap_pstrcat(db->pool, buf, "first_name='", ptr->first_name, "',", NULL );
   if ( ptr->initials )
      buf = ap_pstrcat(db->pool, buf, "initials='", ptr->initials, "',", NULL );
   if ( ptr->email_address )
      buf = ap_pstrcat(db->pool, buf, "email_address='", ptr->email_address, "',", NULL );

   if ( ptr->is_active )
      buf = ap_pstrcat(db->pool, buf, "is_active=true,", NULL );
   else
      buf = ap_pstrcat(db->pool, buf, "is_active=false,", NULL );

   len = strlen(buf);
   if ( len>0 )  buf = ap_pstrndup(db->pool, buf, len - 1);

   buf = ap_pstrcat(db->pool, buf, " WHERE user_oid = ", ptr->oid, ";", NULL);
   return buf;
}

static char* insert_request(database *db, new_request* ptr)
{
  
   if (!ptr->title  || !ptr->request) return NULL;

   ptr->title = (char*) escape_single_quotes(db->pool, ptr->title );
   ptr->request = (char*) escape_single_quotes(db->pool, ptr->request );

   return ap_pstrcat( db->pool, "INSERT INTO requests (", 
		"request_oid,"
		"title, ",
		"request, ",
		"feature_oid, ",
		"request_type_oid, ",
		"submitter_oid, ",
		"severity_oid, ",
		"reproducibility_oid, ",
		"platform_oid, ",
		"product_oid, ",
		"originator_oid,",
		"component_oid, ",
		"comp_rev_oid, ",
		"finding_act_oid,",
		"finding_mode_oid, ",
		"affects_doc_oid",
		") VALUES (",  
		ptr->oid, ",'",
		ptr->title, "', '",
		ptr->request, "', ", 
		ptr->feature_oid, ", ",
		ptr->request_type_oid, ", ",
		ptr->submitter_oid,  ", ",
		ptr->severity_oid, ", ",
		ptr->reproducibility_oid, ", ",
		ptr->platform_oid, ", ",
		ptr->product_oid, ", ",
		ptr->originator_oid, ", ",
		ptr->component_oid, ", ",
		ptr->comp_rev_oid, ", ",
		ptr->finding_act_oid, ", ",
		ptr->finding_mode_oid, ", ",
		ptr->affects_doc_oid, 
		");",  NULL );
}

static char* update_request(database *db, request_update* ptr)
{
   int   len;
   char* buf= "UPDATE requests SET ";

   if (!ptr) return NULL;
   if (!ptr->request_oid) return NULL;

   if ( ptr->feature_oid )
      buf = ap_pstrcat(db->pool, buf, " feature_oid=", ptr->feature_oid, ",", NULL );
   if ( ptr->request_type_oid )
      buf = ap_pstrcat(db->pool, buf, " request_type_oid=", ptr->request_type_oid, ",", NULL );
   if ( ptr->assigned_to_oid )
      buf = ap_pstrcat(db->pool, buf, " assigned_to_oid=", ptr->assigned_to_oid, ",", NULL );
   if ( ptr->verified_by_oid )
      buf = ap_pstrcat(db->pool, buf, " verified_by_oid=", ptr->verified_by_oid, ",", NULL );
   if ( ptr->status_oid )
      buf = ap_pstrcat(db->pool, buf, " status_oid=", ptr->status_oid, ",", NULL );
   if ( ptr->severity_oid )
      buf = ap_pstrcat(db->pool, buf, " severity_oid=", ptr->severity_oid, ",", NULL );
   if ( ptr->priority_oid )
      buf = ap_pstrcat(db->pool, buf, " priority_oid=", ptr->priority_oid, ",", NULL );
   if ( ptr->reproducibility_oid )
      buf = ap_pstrcat(db->pool, buf, " reproducibility_oid=", ptr->reproducibility_oid, ",", NULL );
   if ( ptr->platform_oid )
      buf = ap_pstrcat(db->pool, buf, " platform_oid=", ptr->platform_oid, ",", NULL );
   if ( ptr->product_oid )
      buf = ap_pstrcat(db->pool, buf, " product_oid=", ptr->product_oid, ",", NULL );
   if ( ptr->originator_oid )
      buf = ap_pstrcat(db->pool, buf, " originator_oid=", ptr->originator_oid, ",", NULL );
   if ( ptr->component_oid )
      buf = ap_pstrcat(db->pool, buf, " component_oid=", ptr->component_oid, ",", NULL );
   if ( ptr->comp_rev_oid )
      buf = ap_pstrcat(db->pool, buf, " comp_rev_oid=", ptr->comp_rev_oid, ",", NULL );
   if ( ptr->fixed_in_comp_rev_oid )
      buf = ap_pstrcat(db->pool, buf, " fixed_in_comp_rev_oid=", ptr->fixed_in_comp_rev_oid, ",", NULL );
   if ( ptr->finding_act_oid )
      buf = ap_pstrcat(db->pool, buf, " finding_act_oid=", ptr->finding_act_oid, ",", NULL );
   if ( ptr->finding_mode_oid )
      buf = ap_pstrcat(db->pool, buf, " finding_mode_oid=", ptr->finding_mode_oid, ",", NULL );
   if ( ptr->marketing_rev_oid )
      buf = ap_pstrcat(db->pool, buf, " marketing_rev_oid=", ptr->marketing_rev_oid, ",", NULL );
   if ( *ptr->level_of_effort )
      buf = ap_pstrcat(db->pool, buf, " level_of_effort='", ptr->level_of_effort, "',", NULL );
   if ( ptr->pam_release_oid )
      buf = ap_pstrcat(db->pool, buf, " pam_release_oid=", ptr->pam_release_oid, ",", NULL );
   if ( ptr->affects_doc_oid )
      buf = ap_pstrcat(db->pool, buf, " affects_doc_oid=", ptr->affects_doc_oid, ",", NULL );
   if ( ptr->resolution_oid )
      buf = ap_pstrcat(db->pool, buf, " resolution_oid=", ptr->resolution_oid, ",", NULL );
   if ( ptr->resolution )
      buf = ap_pstrcat(db->pool, buf, " resolution='", ptr->resolution, "',", NULL );
   if ( ptr->request )
      buf = ap_pstrcat(db->pool, buf, " request='", ptr->request, "',", NULL );
   if ( ptr->verification )
      buf = ap_pstrcat(db->pool, buf, " verification='", ptr->verification, "',", NULL );

//   if ( ptr->release_date )
//      buf = ap_pstrcat(db->pool, buf, " release_date='", ptr->release_date, "',", NULL );

   len = strlen(buf);
   if (len>0)  {
      buf = ap_pstrndup(db->pool, buf, len - 1);
      buf = ap_pstrcat(db->pool, buf, ", last_modified = now()", NULL );
      buf = ap_pstrcat(db->pool, buf, " WHERE request_oid = ", ptr->request_oid, ";", NULL);
   }
   return buf;
}

static char* insert_component(database *db, component* ptr)
{
   return ap_pstrcat( db->pool, "INSERT INTO components (", 
		"component_oid, ", 
		"component, ",
		"comp_type_oid, ",
		"user_oid ",
		") VALUES (",  
		ptr->oid, ",'",
		ptr->name, "', ",
		ptr->comp_type_oid, ", ",
		ptr->user_oid, 
		");",  NULL );
}


static char* update_component(database *db, component* ptr)
{
   return ap_pstrcat( db->pool, "UPDATE components SET ", 
		"component='", ptr->name, "', ",
		"comp_type_oid=", ptr->comp_type_oid, ", "
		"user_oid=", ptr->user_oid, 
		" WHERE component_oid = ",
                ptr->oid,
		";",  NULL );
}

static char* insert_market_rev(database *db, new_market_rev* ptr)
{
   return ap_pstrcat( db->pool, "INSERT INTO marketing_revisions (", 
		"marketing_rev_oid,",
		"marketing_rev,",
		"product_oid",
		") VALUES (",  
 		ptr->oid, ", '",
		ptr->value, "', ",
		ptr->fkey, 
		");",  NULL );
}

static char* update_market_rev(database *db, new_market_rev* ptr)
{
   return ap_pstrcat( db->pool, "UPDATE marketing_revisions SET ", 
		"marketing_rev='", ptr->value, "', ",
		"product_oid=", ptr->fkey, 
		" WHERE marketing_rev_oid = ",
                ptr->oid,
		";",  NULL );
}

static char* insert_comp_rev(database *db, new_comp_rev* ptr)
{
   return ap_pstrcat( db->pool, "INSERT INTO component_revisions (", 
		"comp_rev_oid,",
		"comp_rev,",
		"component_oid",
		") VALUES (",  
		ptr->oid, ",'", 
		ptr->value, "', ",
		ptr->fkey, 
		");",  NULL );
}

static char* update_comp_rev(database *db, new_comp_rev* ptr)
{
   return ap_pstrcat( db->pool, "UPDATE component_revisions SET ", 
		"comp_rev='", ptr->value, "' ",
		" WHERE comp_rev_oid = ", ptr->oid,
		";",  NULL );
}

static char* insert_pam_release(database *db, new_pam_release* ptr )
{
   return ap_pstrcat( db->pool, "INSERT INTO pam_releases (", 
		"pam_release_oid,",
		"pam_release,",
		"product_oid",
		") VALUES (",  
		ptr->oid, ", '",
		ptr->value, "', ",
		ptr->fkey, 
		");",  NULL );
}

static char* update_pam_release(database *db, new_pam_release* ptr)
{
   return ap_pstrcat( db->pool, "UPDATE pam_releases SET ", 
		"pam_release='", ptr->value, "', ",
		"product_oid=", ptr->fkey, 
		" WHERE pam_release_oid = ",
                ptr->oid,
		";",  NULL );
}

static char* insert_status_history(database *db, new_history* ptr)
{
   return ap_pstrcat( db->pool, "INSERT INTO status_history (", 
		"status,",
		"request_oid,",
		"user_oid",
		") VALUES (",  
		ptr->value, ", ",
		ptr->request_oid, ", ", 
		ptr->user_oid, 
		");",  NULL );
}

static char* insert_comment(database *db, new_history* ptr)
{
   return ap_pstrcat( db->pool, "INSERT INTO comments (",
		"comment,",
		"request_oid,",
		"user_oid",
		") VALUES ('",  
		ptr->value, "', ",
		ptr->request_oid, ", ", 
		ptr->user_oid, 
		");",  NULL );
}

static char* fetch_next_sequence(database* db, CLASSES c) 
{
   return fetch_str(db, get_seq_sql(db, c) );
}

int db_save(database *db, CLASSES c, void *ptr)
{
   char* sql=NULL;
   char* seq=NULL;  
   data_ptr uptr;

   if (!ptr) return 0;

   switch (c) {

   case CL_SYS_USERS:
      uptr.su = (system_users*) ptr;
      sql = update_system_users(db, uptr.su);
      break;

   case CL_USER:
     
      uptr.u = (user*)ptr; 

//      // Truncate data that exceeds maximum allowed:
//      if (uptr.u->account && strlen(uptr.u->account) > MAX_USER_ACCOUNT )
//         uptr.u->account = ap_pstrndup(db->pool,  uptr.u->account, MAX_USER_ACCOUNT );	
//      if (uptr.u->password && strlen(uptr.u->password) > MAX_PASSWORD  )
//         uptr.u->password = ap_pstrndup(db->pool, uptr.u->password, MAX_PASSWORD );
//      if (uptr.u->last_name && strlen(uptr.u->last_name) > MAX_LAST_NAME  )
//         uptr.u->last_name = ap_pstrndup(db->pool, uptr.u->last_name, MAX_LAST_NAME );
//      if (uptr.u->first_name && strlen(uptr.u->first_name) > MAX_FIRST_NAME  )
//         uptr.u->first_name = ap_pstrndup(db->pool, uptr.u->first_name, MAX_FIRST_NAME );
//      if (uptr.u->initials && strlen(uptr.u->initials) > MAX_INITIALS  )
//         uptr.u->initials = ap_pstrndup(db->pool, uptr.u->initials, MAX_INITIALS );
//      if (uptr.u->email_address && strlen(uptr.u->email_address) > MAX_EMAIL  )
//         uptr.u->email_address = ap_pstrndup(db->pool, uptr.u->email_address, MAX_EMAIL );
         
      // Prepare data for sql 
      uptr.u->account		= (char*) escape_single_quotes(db->pool, uptr.u->account );
      uptr.u->password		= (char*) escape_single_quotes(db->pool, uptr.u->password );
      uptr.u->last_name		= (char*) escape_single_quotes(db->pool, uptr.u->last_name );
      uptr.u->first_name	= (char*) escape_single_quotes(db->pool, uptr.u->first_name );
      uptr.u->initials		= (char*) escape_single_quotes(db->pool, uptr.u->initials );
      uptr.u->email_address	= (char*) escape_single_quotes(db->pool, uptr.u->email_address );

      if (uptr.u->oid) 
      {
         // Update 
         sql = update_user(db, uptr.u );
         log(sql);
      } else {
         // Insert
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.u->oid = seq;
         sql = insert_user(db, uptr.u);
         log(sql);
      }
      break;

   case CL_COMPONENT:
      uptr.c = (component*) ptr; 

      if (strlen(uptr.c->name) > MAX_COMPONENT)
         uptr.c->name = ap_pstrndup(db->pool, uptr.c->name, MAX_COMPONENT);

      uptr.c->name = (char*) escape_single_quotes(db->pool, uptr.c->name );

      if (uptr.c->oid) {
         sql = update_component(db, uptr.c);
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.c->oid = seq;
         sql = insert_component(db, uptr.c);
      }
      break;
      
   case CL_COMP_TYPE:
      uptr.t = (tag*) ptr;

      if (strlen(uptr.t->value) > MAX_COMP_TYPE)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_COMP_TYPE);

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );

      if (uptr.t->oid)
      {
          sql = ap_pstrcat( db->pool, "UPDATE component_types SET comp_type ='",  
		uptr.t->value , "' WHERE comp_type_oid = ", 
		uptr.t->oid, ";",  NULL );
      } else {

         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO component_types ( comp_type_oid, comp_type ) VALUES (",  
		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_COMP_REV:
      uptr.nv  = (new_value_key*) ptr;

      if ( strlen(uptr.nv->value) > MAX_COMP_REV)
         uptr.nv->value = ap_pstrndup(db->pool,  uptr.nv->value, MAX_COMP_REV);

      uptr.nv->value = (char*) escape_single_quotes(db->pool, uptr.nv->value );
      log("CL_COMP_REV before if");
      if (uptr.nv->oid)
      {
         log("update comp rev");
         sql = update_comp_rev(db, uptr.nv );
      } else {
         log("insert comp rev");
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.nv->oid = seq;
         sql = insert_comp_rev(db, uptr.nv);
      }
      break;

   case CL_PRODUCT:
      uptr.t = (tag*) ptr;

      if(strlen(uptr.t->value) > MAX_PRODUCT)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_PRODUCT );

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );
      if (uptr.t->oid)
      {
         sql = ap_pstrcat( db->pool, "UPDATE products SET product ='",  
		uptr.t->value, "' WHERE product_oid = ", 
		uptr.t->oid, ";",  NULL );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO products ( product_oid, product ) VALUES (",  
		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_PAM:
      uptr.nv = (new_pam_release*) ptr;

      if(strlen(uptr.nv->value) > MAX_PAM_RELEASE)
         uptr.nv->value= ap_pstrndup(db->pool, uptr.nv->value, MAX_PAM_RELEASE );

      uptr.nv->value = (char*) escape_single_quotes(db->pool, uptr.nv->value);
      if (uptr.nv->oid)
      {
         sql = update_pam_release(db, uptr.nv );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.nv->oid = seq;
         sql = insert_pam_release(db, uptr.nv);
      }
      break;

   case CL_MARKET_REV:
      uptr.nv = (new_market_rev*) ptr;

      if(strlen(uptr.nv->value) > MAX_MARKET_REV)
         uptr.nv->value= ap_pstrndup(db->pool, uptr.nv->value, MAX_MARKET_REV);

      uptr.nv->value = (char*) escape_single_quotes(db->pool, uptr.nv->value);
      if (uptr.nv->oid)
      {
         sql = update_market_rev(db, uptr.nv );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.nv->oid = seq;
         sql = insert_market_rev(db, uptr.nv);
      }
      break;

   case CL_FINDING_ACT:
      uptr.t = (tag*) ptr;

      if(strlen(uptr.t->value) > MAX_FINDING_ACT)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_FINDING_ACT );

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );
      if (uptr.t->oid)
      {
         sql = ap_pstrcat( db->pool, "UPDATE finding_activities  SET finding_activity ='",  
   		uptr.t->value, "' WHERE finding_act_oid = ", 
   		uptr.t->oid, ";",  NULL );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO finding_activities ( finding_act_oid, finding_activity ) VALUES (",  
   		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_ORIG:
      uptr.t = (tag*) ptr;

      if(strlen(uptr.t->value) > MAX_ORIGINATOR)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_ORIGINATOR );

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );
      if (uptr.t->oid)
      {
         sql = ap_pstrcat( db->pool, "UPDATE  originators SET originator='",  
   		(char*) uptr.t->value , "' WHERE originator_oid=", 
   		uptr.t->oid, ";",  NULL );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO originators ( originator_oid, originator ) VALUES (",
   		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_PLATFORM:
      uptr.t = (tag*) ptr;

      if(strlen(uptr.t->value) > MAX_PLATFORM)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_PLATFORM );

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );
      if (uptr.t->oid)
      {
         sql = ap_pstrcat( db->pool, "UPDATE platforms  SET platform ='", uptr.t->value, 
			"' WHERE platform_oid = ", uptr.t->oid, ";",  NULL );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO platforms ( platform_oid, platform ) VALUES (", 
   		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_RESOLUTION:
      uptr.t = (tag*) ptr;

      if(strlen(uptr.t->value) > MAX_RESOLUTION)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_RESOLUTION );

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );
      if (uptr.t->oid)
      {
         sql = ap_pstrcat( db->pool, "UPDATE resolutions  SET resolution ='",  
   		uptr.t->value, "' WHERE resolution_oid = ", 
   		uptr.t->oid, ";",  NULL );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO resolutions ( resolution_oid, resolution ) VALUES (",  
   		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_REQUEST_TYPE:
      uptr.t = (tag*) ptr;

      if(strlen(uptr.t->value) > MAX_REQUEST_TYPE)
         uptr.t->value= ap_pstrndup(db->pool, uptr.t->value, MAX_REQUEST_TYPE );

      uptr.t->value = (char*) escape_single_quotes(db->pool, uptr.t->value );
      if (uptr.t->oid)
      {
         sql = ap_pstrcat( db->pool, "UPDATE request_types SET request_type ='",  
   		uptr.t->value, "' WHERE request_type_oid = ", 
   		uptr.t->oid, ";",  NULL );
      } else {
         seq = fetch_next_sequence(db, c);
         if(!seq) return 0;
         uptr.t->oid = seq;
         sql = ap_pstrcat( db->pool, "INSERT INTO request_types ( request_type_oid, request_type ) VALUES (",  
   		uptr.t->oid, ",'", uptr.t->value, "');",  NULL );
      }
      break;

   case CL_NEW_REQUEST:
   
      uptr.nr = (new_request*) ptr;

      if(strlen(uptr.nr->title) > MAX_REQUEST_TITLE)
         uptr.nr->title= ap_pstrndup(db->pool, uptr.nr->title, MAX_REQUEST_TITLE );

      uptr.nr->title = (char*) escape_single_quotes(db->pool, uptr.nr->title );
      uptr.nr->request = (char*) escape_single_quotes(db->pool, uptr.nr->request );

      seq = fetch_next_sequence(db, c);
      if(!seq) return 0;
      uptr.nr->oid = ap_pstrdup(db->pool, seq);
      sql = insert_request(db, uptr.nr);
      log(sql);
      break;

   case CL_REQUEST:
      uptr.r  = (request_update*) ptr;

      if(strlen(uptr.r->level_of_effort) > MAX_LEVEL_OF_EFFORT)
         uptr.r->level_of_effort= ap_pstrndup(db->pool, uptr.r->level_of_effort, MAX_LEVEL_OF_EFFORT);

      uptr.r->level_of_effort = (char*) escape_single_quotes(db->pool, uptr.r->level_of_effort );

      uptr.r->request = (char*) escape_single_quotes(db->pool, uptr.r->request );
      uptr.r->resolution = (char*) escape_single_quotes(db->pool, uptr.r->resolution );
      uptr.r->verification = (char*) escape_single_quotes(db->pool, uptr.r->verification );

      sql = update_request(db, uptr.r );
      break;

   case CL_COMMENT: 
      uptr.nh = (new_history*) ptr; 
      uptr.nh->value = (char*) escape_single_quotes(db->pool, uptr.nh->value);
      sql = insert_comment(db, uptr.nh);
      break;

   case CL_STATUS_HISTORY:
      uptr.nh = (new_history*) ptr; 
      uptr.nh->value = (char*) escape_single_quotes(db->pool, uptr.nh->value );
      sql = insert_status_history(db, (new_history*) uptr.nh);
      break;

   default:
      set_error_no(db, 1000);
      set_error_msg(db, "Unknown class requested"); 
      return 0;
   }

   return execute_sql(db, sql );
}
///////////////////////////

char* db_authenticates_user(database *db, char* account, char* password)
{
   char* sql;

   sql =ap_pstrcat(db->pool, "select user_oid from users where user_account='", 
		account, "' and password='", password, "';", NULL);

   return fetch_str(db, sql);
}

static char* format_date(ap_pool *pool, char* val)
{
   int i;
   char* day=NULL;
   char* month=NULL;
   char* year=NULL;
   char* ptr=NULL;

   if (!val) return NULL;
   ptr = val;
 
   year = ap_pstrndup(pool, ptr, 4);  
   for(i=0;i<5;i++) ++ptr; 
   month = ap_pstrndup(pool, ptr, 2);
   for(i=0;i<3;i++) ++ptr; 
   day = ap_pstrndup(pool, ptr, 2);

   return ap_pstrcat(pool, month, "/", day, "/", year, NULL);
}

static system_users* fetch_system_users(database *db)
{
   twodim *td;
   system_users* ptr=NULL;

   td = recordset(db, "select comp_super_oid, qa_mgr_oid, 
	market_mgr_oid from system where system_oid=1;", 3); 

   if(!td) return NULL;

   ptr = (system_users*) ap_palloc(db->pool, sizeof(system_users) );
   if (!ptr) return NULL;

   ptr->comp_super_oid	= td_get_val(td, 0, 0);
   ptr->qa_mgr_oid	= td_get_val(td, 0, 1);
   ptr->market_mgr_oid	= td_get_val(td, 0, 2);
   return ptr;
}

static char* update_system_users(database *db, system_users* ptr)
{
   return ap_pstrcat( db->pool, "UPDATE system SET ", 
		"comp_super_oid=", ptr->comp_super_oid, ", ",
		"qa_mgr_oid=", ptr->qa_mgr_oid, ", "
		"market_mgr_oid=", ptr->market_mgr_oid, 
		" WHERE system_oid = 1;",
		NULL );
}
