/*
 * shrs.h
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

#ifndef _SHRS_H
#define  _SHRS_H 

#define DEFAULT_ENCTYPE "application/x-www-form-urlencoded"

typedef struct {
   char* comp_super_oid;
   char* qa_mgr_oid;
   char* market_mgr_oid;
} system_users;

typedef struct {
   char* key;
   char* user_oid;
} session;

typedef struct {
   char *request_oid;
   char *session_user_oid;

   char *qa_mgr_oid;
   char *comp_super_oid;
   char *market_mgr_oid;

   char *submitter_oid;
   char *assigned_to_oid;
   char *verified_by_oid;
   char *comp_mgr_oid;
} email_canidates;

typedef enum {
   STATUS_ZERO = 0,
   STATUS_NEW, 
   STATUS_OPEN, 
   STATUS_ASSIGNED, 
   STATUS_FIXED, 
   STATUS_IN_TEST, 
   STATUS_VERIFIED,
   STATUS_REQUEST_INFO, 
   STATUS_DECLINED, 
   STATUS_WITHDRAWN, 
   STATUS_CLOSED, 
   TOTAL_STATES	
} STATUS;

typedef struct {
   char* request_oid;
   char* user_oid;
   char* lockedby; 
} checkout;


typedef struct {
   const char* oid;
   const char* status;
   const char* severity;
   const char* priority;
   const char* assigned_to;
   const char* component;
   const char* revision;
   const char* title;
   const char* request;
} summary;

typedef struct {
   int size;
   summary** pp;
} summaries;


typedef struct {
   char* oid;
   char* account;
   char* password;
   char* is_active;
} user_account;

typedef struct {
   int qa_mgr;
   int comp_super;
   int comp_mgr;
   int market_mgr;
   int submitter;
   int assigned_to;
   int verified_by;
} user_type;

typedef struct {
   char* oid;
   char* account;
   char* password;
   char* last_name;
   char* first_name;
   char* initials;
   char* email_address;
   int is_qa_mgr;
   int is_comp_sup;
   int is_market_mgr;
   int is_active;
  // int is_tracking;
} user;

typedef struct {
   char* oid;
   char* value;
} tag;

typedef struct {
   char* oid;
   tag user;
   char* entrydate;
} tag_user;

typedef struct {
    char* oid;
    char* title;
    char* feature_oid;
    char* submitter_oid;   
    char* finding_act_oid;
    char* platform_oid;
    char* originator_oid;
    char* request_type_oid;
    char* product_oid;
    char* component_oid;
    char* comp_rev_oid;
    char* severity_oid;
    char* reproducibility_oid;
    char* finding_mode_oid;
    char* affects_doc_oid;
    char* request;
 } new_request;

typedef struct {
   char* oid;
   char* value;
   char* fkey;
}  new_value_key, new_market_rev, new_comp_rev, new_pam_release;

typedef struct {
   char* oid;
   char* name;
   char* comp_type_oid;
   char* user_oid ;
} component; 

typedef struct {
   char* value;
   char* request_oid;
   char* user_oid;
} new_history;

typedef struct {
   char* request_oid;
   char* feature_oid;
   char* request_type_oid;
   char* assigned_to_oid;
   char* verified_by_oid;
   char* status_oid;
   char* severity_oid;
   char* priority_oid;
   char* reproducibility_oid;
   char* platform_oid;
   char* product_oid;
   char* originator_oid;
   char* component_oid;
   char* comp_rev_oid;
   char* fixed_in_comp_rev_oid;
   char* finding_act_oid;
   char* finding_mode_oid;
   char* marketing_rev_oid;
   char* level_of_effort;
   char* pam_release_oid;
   char* affects_doc_oid;
   char* resolution_oid;
   char* resolution;
   char* release_date;
   char* request;
   char* verification;
} request_update; 


typedef struct {
   char* entrydate; 
   char* user_name;
   const char* text;
} single_entry;

typedef struct {
   int size;
   single_entry **pp;
} entries;


typedef struct {

   char* oid;
   char* date_submitted;
   char* date_last_modified;
   char* release_date;

   char* title;

   tag feature;

   tag submitter;   
   tag assigned_to;
   tag verified_by;

   tag platform;
   tag originator;
   tag resolution;
   tag request_type;
   tag finding_activity;

   tag product;
   tag pam_release;
   tag market_rev;

   tag component;
   tag comp_rev;
   tag comp_mgr;

   char* comp_type;
   
   tag fixed_in_comp_rev;

   tag	status;
   tag	priority;
   tag	severity;
   tag	reproducibility;
   tag	finding_mode;
   tag	affects_docs;

   char*    level_of_effort;

   char*    desc_request;
   char*    desc_resolution;
   char*    desc_verification;

   entries 	*notes;
   entries	*status_history;

} request;


#define A_MAIN				0
#define A_NEW_REQUEST_PAGE_1		1
#define A_NEW_REQUEST_PAGE_2		2
#define A_NEW_REQUEST_PAGE_3		3
#define A_SAVE_NEW_REQUEST		4
#define A_RECORD_REQUEST		5
#define A_REQUEST			6
#define A_SAVE_REQUEST			7
#define A_FILTER			8
#define A_SAVE_FILTER			9
#define A_SUMMARY			20
#define A_REPORT			21
#define A_LOGIN				22
#define A_ADMIN				23
#define A_USER				24
#define A_SELECT_USER			25
#define A_ADMIN_USER			26
#define A_SAVE_USER			27
#define A_COMP				28
#define A_SELECT_COMP			29
#define A_ADMIN_COMP			30
#define A_SAVE_COMP			31
#define A_COMP_REV			32
#define A_SELECT_COMP_REV   		33
#define A_ADD_COMP_REV			34
#define A_EDIT_COMP_REV			35
#define A_SAVE_COMP_REV			36
#define A_COMP_TYPE			37
#define A_SELECT_COMP_TYPE   		38
#define A_ADD_COMP_TYPE			39
#define A_EDIT_COMP_TYPE		40
#define A_SAVE_COMP_TYPE		41
#define A_PRODUCT			42
#define A_ADD_PRODUCT			43
#define A_SELECT_PRODUCT		44
#define A_ADMIN_PRODUCT			45
#define A_MARKETING_REV			46
#define A_SELECT_MARKETING_REV 		47
#define A_ADD_MARKETING_REV		48
#define A_EDIT_MARKETING_REV		49
#define A_SAVE_MARKETING_REV		50
#define A_PAM_RELEASE			51
#define A_ADD_PAM_RELEASE		52
#define A_SELECT_PAM_RELEASE   		53
#define A_EDIT_PAM_RELEASE		54
#define A_SAVE_PAM_RELEASE		55
#define A_PLATFORM			56
#define A_ADD_PLATFORM			57
#define A_EDIT_PLATFORM			58
#define A_SAVE_PLATFORM			59
#define A_ORIGINATOR			60
#define A_SELECT_ORIGINATOR		61
#define A_ADD_ORIGINATOR		62
#define A_EDIT_ORIGINATOR		63
#define A_SAVE_ORIGINATOR		64
#define A_RESOLUTION			65
#define A_SELECT_RESOLUTION		66
#define A_ADD_RESOLUTION		67
#define A_EDIT_RESOLUTION		68
#define A_SAVE_RESOLUTION		69
#define A_FINDING_ACT			70
#define A_SELECT_FINDING_ACT		71
#define A_ADD_FINDING_ACT		72
#define A_EDIT_FINDING_ACT		73
#define A_SAVE_FINDING_ACT		74
#define A_REQUEST_TYPE			75
#define A_SELECT_REQUEST_TYPE		76
#define A_ADD_REQUEST_TYPE		77
#define A_EDIT_REQUEST_TYPE		78
#define A_SAVE_REQUEST_TYPE		79
#define A_CHG_PWD			80
#define A_SAVE_PRODUCT			81
#define A_LOGOUT			82
#define A_CHECKIN_REQUEST		83
#define A_SYS_USERS			84
#define A_SAVE_SYS_USERS		85
#define A_CLEAR_ALL_LOCKS		86
////////////////////////////////////////
// Main switch actions
#endif
////////////////////////////////////////////////////////////
