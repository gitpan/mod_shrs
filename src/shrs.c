/*
 * shrs.c
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


#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"
#include "http_log.h"
#include "apache_request.h"

#include "shrs.h"
#include "db_shrs.h"
#include "form.h"
#include "form_menus.h"
#include "form_request.h"
#include "form_admin.h"
#include "form_summary.h"


int shrs_handler(request_rec *r ) 
{
    int 		status = OK;
    FILE	 	*file;
    ApacheRequest 	*req;
    database 		db;
    session		*sid;
    char		*user_oid;
    long 		l_formaction = -1;

    // Posted Parameters
    char		*key;
    char		*account;
    char		*password;
    char		*formaction;


   if (r->header_only) return DECLINED;
   if (r->method_number != M_POST) return DECLINED; 

   req = ApacheRequest_new(r);
   status = ApacheRequest_parse(req);
   if ( status != OK) return status;

   db_construct(r, &db);
   if (!db_connect(&db, "shrs") )
   {
      return sh_outoforder(r, "database failed to connect");
   }

   db_delete_old_sessions(&db);

   key = (char*)ApacheRequest_param(req, "sid");
   if (key)
   {
      sid =  db_fetch_session(&db, key);
      if (!sid)  {
         db_close_connection(&db);
         return sh_message_login(r, "Your session timed out");   
      }

      db_extend_session(&db, key);

      formaction = (char*) ApacheRequest_param(req, "formaction");
      if (formaction) {
         l_formaction = atol(formaction);
      } else {
         db_close_connection(&db);
         return sh_message_login(r, "Declined");   
      }

   } else {

      account = (char*)ApacheRequest_param(req, "login");
      if(!account) {
         db_close_connection(&db);
         return sh_message_login(r, "Declined");   
      }

      password = (char*)ApacheRequest_param(req, "password");
      if(!password)  {
         db_close_connection(&db);
         return sh_message_login(r, "Declined");   
      }

      user_oid = db_authenticates_user(&db, account, password);
      if (user_oid)
      {
          l_formaction = A_MAIN;  
          sid = (session*) ap_palloc(r->pool, sizeof(session) ); 
          if(!sid) {
             db_close_connection(&db);
             return sh_outoforder(r, "main switch, apache failed to allocate sid");
          } 

          sid->user_oid = ap_pstrdup(r->pool, user_oid);
          if ( !db_new_session(&db, sid))
          {
             db_close_connection(&db);
             return sh_outoforder(r, "main switch, database failed to create new session" );
          } 
   
      } else {
         db_close_connection(&db);
         return sh_message_login(r, "Login Failed");   
      }
   } // End else

switch ( l_formaction ) {

    case A_CHECKIN_REQUEST:

        h_ph_checkin_request(req, sid, &db);
        // This case intentionally drops into the main menu.
 
    case A_MAIN:
       status = h_sh_mainmenu(r, sid);
       break;
 
    case A_NEW_REQUEST_PAGE_1:
       status = h_sh_new_request_page1(r, sid, &db);
       break;
 
    case A_NEW_REQUEST_PAGE_2:
       status = h_sh_new_request_page2(r, sid, req, &db );
       break;
 
    case A_NEW_REQUEST_PAGE_3:
       status = h_sh_new_request_page3(r, sid, req, &db );
       break;
 
    case A_SAVE_NEW_REQUEST:
       status = h_ph_save_new_request(r, sid, req, &db);
	break;
 
    case A_RECORD_REQUEST:
       status = h_sh_record_request(r, sid, NULL);
       break;
 
    case A_REQUEST:
       status = h_sh_request(r, sid, req, &db);
       break;
 
    case A_SAVE_REQUEST:
	status = h_ph_save_request(r, sid, req, &db);
	break;

    case A_FILTER:
       status = sh_message_return(r, sid->key, "Main Switch Board", "Not Implemented", A_MAIN);
       break;
 
    case A_SAVE_FILTER:
       status = sh_message_return(r, sid->key, "Main Switch Board", "Not Implemented", A_MAIN);
       break;
 
    case A_SUMMARY:
       status = h_sh_summary(r,  sid, req,  &db);
       break;
 
    case A_REPORT:
       status = sh_message_return(r, sid->key, "Main Switch Board", "Not Implemented", A_MAIN);
       break;
 
    case A_CHG_PWD:
       status = sh_message_return(r, sid->key, "Main Switch Board", "Not Implemented", A_MAIN);
       break;
 
   case A_LOGOUT:
      db_delete_session(&db, sid->key);      
      status = h_sh_login(r);   

//      r->uri = ap_pstrdup(r->pool, "/");
//      r->path_info = ap_pstrdup(r->pool, "/");
//      r->filename = ap_pstrdup(r->pool, "index.html");
//      file = ap_pfopen(r->pool, r->filename, "r");
//      if (file == NULL) {
//         status = NOT_FOUND;
//      } else {
//         ap_send_http_headers(r);
//         ap_send_fd(file, r);
//         ap_pclose(r->pool, file);
//         status = OK;
//      } 
      

      break;

   case A_ADMIN:
      status = h_sh_admin(r, sid);
      break;

 
   case A_USER:
      status = h_sh_add_or_edit(r, sid, req, "Admin User Accounts", CL_USER, 
		A_ADMIN_USER, A_SELECT_USER, A_ADMIN);
      break;
   case A_SELECT_USER:
      status = h_sh_select_to_continue(r, sid, &db, CL_USER_ACT, 
		"Select A User Account To Edit", A_ADMIN_USER, A_USER);
      break;
   case A_ADMIN_USER:
      status = h_sh_admin_user(r, sid, req, &db);
      break;
   case A_SAVE_USER:
	status = h_ph_save_user(r, sid, req, &db);
	break;
 

    case A_COMP:
	status = h_sh_add_or_edit(r, sid, req, "Admin Components", CL_COMPONENT, 
		A_ADMIN_COMP, A_SELECT_COMP, A_ADMIN);
        break;
    case A_SELECT_COMP:
        status = h_sh_select_to_continue(r, sid, &db, CL_COMPONENT, 
		"Select A Component To Edit", A_ADMIN_COMP, A_COMP);
        break;
    case A_ADMIN_COMP:
	status = h_sh_admin_comp(r, sid, req, &db);
       break;
    case A_SAVE_COMP:
	status = h_ph_save_component(r, sid, req, &db);
       break;
 
 
    case A_COMP_REV:
       status = h_sh_add_or_edit(r, sid, req, "Component Revisions", CL_COMP_REV, 
		A_ADD_COMP_REV, A_EDIT_COMP_REV, A_COMP);
       break;
    case A_ADD_COMP_REV:
       status = h_sh_add_lookup(r, sid, req, &db, CL_COMP_REV, "Component Revision", 
		A_SAVE_COMP_REV, A_COMP_REV, MAX_COMP_REV);
       break;
    case A_SELECT_COMP_REV:
        status = h_sh_select_to_continue(r, sid, &db, CL_COMP_REV, 
		"Select A Component To Edit", A_COMP_REV, A_ADMIN);
        break;
    case A_EDIT_COMP_REV:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_COMP_REV, "Component Revision", 
		A_SAVE_COMP_REV, A_COMP_REV, MAX_COMP_REV);
       break;
    case A_SAVE_COMP_REV:
       status = h_ph_value_key(r, sid, req, &db, CL_COMP_REV, A_COMP_REV, A_COMP_REV);
       break;
 
 
    case A_COMP_TYPE:
	status = h_sh_add_or_edit(r, sid, req, "Admin Component Types", CL_COMP_TYPE,
		A_ADD_COMP_TYPE, A_EDIT_COMP_TYPE, A_COMP);
       break;
    case A_ADD_COMP_TYPE:
       status = h_sh_add_lookup(r, sid, req, &db, CL_COMP_TYPE, "Component Type", 
		A_SAVE_COMP_TYPE, A_COMP_TYPE, MAX_COMP_TYPE);
       break;
    case A_EDIT_COMP_TYPE:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_COMP_TYPE, "Component Type", 
		A_SAVE_COMP_TYPE, A_COMP_TYPE, MAX_COMP_TYPE);
       break;
    case A_SAVE_COMP_TYPE:
       status = h_ph_value_key(r, sid, req, &db, CL_COMP_TYPE, 
		A_COMP_TYPE, A_COMP_TYPE);
       break;
 
 
    case A_PRODUCT:
	status = h_sh_add_or_edit(r, sid, req, "Admin Products", CL_PRODUCT, 
		A_ADMIN_PRODUCT, A_SELECT_PRODUCT, A_ADMIN);
        break;
    case A_SELECT_PRODUCT:
        status = h_sh_select_to_continue(r, sid, &db, CL_PRODUCT, 
		"Select A Product To Edit", A_ADMIN_PRODUCT, A_PRODUCT);
        break;
    case A_ADMIN_PRODUCT:
       status = h_sh_admin_product(r, sid, req, &db);
       break;
    case A_SAVE_PRODUCT:
	status = h_ph_tag(r, sid, req, &db, CL_PRODUCT, A_PRODUCT, A_PRODUCT );
	break;

 
    case A_MARKETING_REV:
       status = h_sh_add_or_edit(r, sid, req, "Admin Marketing Revisions", CL_MARKET_REV,
 		A_ADD_MARKETING_REV, A_EDIT_MARKETING_REV, A_PRODUCT);
       break;
    case A_ADD_MARKETING_REV:
       status = h_sh_add_lookup(r, sid, req, &db, CL_MARKET_REV, "Marketing Revision", 
		A_SAVE_MARKETING_REV, A_MARKETING_REV, MAX_MARKET_REV);
       break;
    case A_SELECT_MARKETING_REV: 
       status = h_sh_select_to_continue(r, sid, &db, CL_PRODUCT, 
		"Select A Product", A_MARKETING_REV, A_ADMIN);
       break;
    case A_EDIT_MARKETING_REV:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_MARKET_REV, "Marketing Revision", 
		A_SAVE_MARKETING_REV, A_MARKETING_REV, MAX_MARKET_REV);
       break;
    case A_SAVE_MARKETING_REV:
         status = h_ph_value_key(r, sid, req, &db, CL_MARKET_REV, 
		A_MARKETING_REV, A_MARKETING_REV);
       break;
 
 
    case A_PAM_RELEASE:
       status = h_sh_add_or_edit(r, sid, req, 
		"Admin Projected Available Marketing Releases (PAM Release)", CL_PAM,
 			A_ADD_PAM_RELEASE, A_EDIT_PAM_RELEASE, A_PRODUCT);
       break;
    case A_ADD_PAM_RELEASE:
      status = h_sh_add_lookup(r, sid, req, &db, CL_PAM, 
		"Projected Available Marketing Releases", 
		A_SAVE_PAM_RELEASE, A_PAM_RELEASE, MAX_PAM_RELEASE);
       break;
    case A_SELECT_PAM_RELEASE:   
       status = h_sh_select_to_continue(r, sid, &db, CL_PRODUCT,
			"Select A Product", A_PAM_RELEASE, A_ADMIN);
       break;
    case A_EDIT_PAM_RELEASE:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_PAM, 
		"Projected Available Marketing Releases", 
		A_SAVE_PAM_RELEASE, A_PAM_RELEASE, MAX_PAM_RELEASE);
       break;
    case A_SAVE_PAM_RELEASE:
         status = h_ph_value_key(r, sid, req, &db, CL_PAM, 
		A_PAM_RELEASE, A_PAM_RELEASE);
       break;

 
    case A_PLATFORM:
       status = h_sh_add_or_edit(r, sid, req, "Admin Platforms", CL_PLATFORM, 
		A_ADD_PLATFORM, A_EDIT_PLATFORM, A_ADMIN);
       break;
    case A_ADD_PLATFORM:
       status = h_sh_add_lookup(r, sid, req, &db, CL_PLATFORM, 
		"Platform", A_SAVE_PLATFORM, A_PLATFORM, MAX_PLATFORM);
       break;
    case A_EDIT_PLATFORM:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_PLATFORM, 
		"Platform", A_SAVE_PLATFORM, A_PLATFORM, MAX_PLATFORM);
       break;
    case A_SAVE_PLATFORM:
         status = h_ph_tag(r, sid, req, &db, CL_PLATFORM, 
		A_PLATFORM, A_PLATFORM);
       break;
 
 
    case A_ORIGINATOR:
       status = h_sh_add_or_edit(r, sid, req, "Admin Originators", CL_ORIG, 
		A_ADD_ORIGINATOR, A_EDIT_ORIGINATOR, A_ADMIN);
       break;
    case A_ADD_ORIGINATOR:
       status = h_sh_add_lookup(r, sid, req, &db, CL_ORIG, "Originator", 
		A_SAVE_ORIGINATOR, A_ORIGINATOR, MAX_ORIGINATOR);
       break;
    case A_EDIT_ORIGINATOR:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_ORIG, "Originator", 
		A_SAVE_ORIGINATOR, A_ORIGINATOR, MAX_ORIGINATOR);
       break;
    case A_SAVE_ORIGINATOR:
       status = h_ph_tag(r, sid, req, &db, CL_ORIG, 
		A_ORIGINATOR, A_ORIGINATOR);
       break;
 
 
    case A_RESOLUTION:
       status = h_sh_add_or_edit(r, sid, req, "Admin Resolutions", CL_RESOLUTION, 
		A_ADD_RESOLUTION, A_EDIT_RESOLUTION, A_ADMIN);
       break;
    case A_ADD_RESOLUTION:
       status = h_sh_add_lookup(r, sid, req, &db, CL_RESOLUTION, "Resolution", 
		A_SAVE_RESOLUTION, A_RESOLUTION, MAX_RESOLUTION);
       break;
    case A_EDIT_RESOLUTION:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_RESOLUTION, "Resolution", 
		A_SAVE_RESOLUTION, A_RESOLUTION, MAX_RESOLUTION);
       break;
    case A_SAVE_RESOLUTION:
       status = h_ph_tag(r, sid, req, &db, CL_RESOLUTION, 
		A_RESOLUTION, A_RESOLUTION);
       break;
 
 
    case A_FINDING_ACT:
       status = h_sh_add_or_edit(r, sid, req, "Admin Finding Activities", CL_FINDING_ACT, 
		A_ADD_FINDING_ACT, A_EDIT_FINDING_ACT, A_ADMIN);
       break;
    case A_ADD_FINDING_ACT:
       status = h_sh_add_lookup(r, sid, req, &db, CL_FINDING_ACT, "Finding Activity", 
		A_SAVE_FINDING_ACT, A_FINDING_ACT, MAX_FINDING_ACT);
       break;
    case A_EDIT_FINDING_ACT:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_FINDING_ACT, "Finding Activity", 
		A_SAVE_FINDING_ACT, A_FINDING_ACT, MAX_FINDING_ACT);
       break;
    case A_SAVE_FINDING_ACT:
       status = h_ph_tag(r, sid, req, &db, CL_FINDING_ACT, 
		A_FINDING_ACT, A_FINDING_ACT);
       break;
 
 
    case A_REQUEST_TYPE:
       status = h_sh_add_or_edit(r, sid, req, "Request Types", CL_REQUEST_TYPE, 
		A_ADD_REQUEST_TYPE, A_EDIT_REQUEST_TYPE, A_ADMIN);
       break;
    case A_ADD_REQUEST_TYPE:
       status = h_sh_add_lookup(r, sid, req, &db, CL_REQUEST_TYPE, "Request Type",
		A_SAVE_REQUEST_TYPE, A_REQUEST_TYPE, MAX_REQUEST_TYPE);
       break;
    case A_EDIT_REQUEST_TYPE:
       status = h_sh_edit_lookup(r, sid, req, &db, CL_REQUEST_TYPE, "Request Type", 
		A_SAVE_REQUEST_TYPE, A_REQUEST_TYPE, MAX_REQUEST_TYPE);
       break;
    case A_SAVE_REQUEST_TYPE:
       status = h_ph_tag(r, sid, req, &db, CL_REQUEST_TYPE,
		A_REQUEST_TYPE, A_REQUEST_TYPE);
       break;
 
    case A_SYS_USERS:
       status = h_sh_admin_system(r, sid, &db);
       break;

    case A_SAVE_SYS_USERS:
       status = h_ph_save_system_users(r, sid, req, &db);
       break;

    case A_CLEAR_ALL_LOCKS:
       status = h_ph_clear_locks(r, sid->key, &db);
       break;

      default:
        status = sh_message_return(r, sid->key, "Main Switch Board", "Unknown Request", A_LOGIN);
   }

   db_close_connection(&db);

   return status;
}
