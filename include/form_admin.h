/*
 * form_admin.h
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

#ifndef _FORM_ADMIN_H
#define _FORM_ADMIN_H

#include "httpd.h"
#include "apache_request.h"
#include "db_shrs.h"


int h_sh_add_lookup(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES class, 
		 char* label,  int next_formaction, int cancel_formaction, int maxlength); 

int h_sh_edit_lookup(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES c, 
		char* label, int next_formaction, int cancel_formaction, int maxlength); 

int h_sh_admin_system(request_rec *r, session* sid, database *db);

int h_sh_admin_user(request_rec *r, session* sid, ApacheRequest *req, database *db);

int h_sh_admin_comp(request_rec *r, session* sid, ApacheRequest *req, database *db);

int h_sh_add_comp_rev(request_rec *r, session* sid);


int h_ph_save_system_users(request_rec *r, session* sid, ApacheRequest *req, database *db);

int h_ph_save_user(request_rec *r, session* sid, ApacheRequest *req, database *db);

int h_ph_save_component(request_rec *r, session* sid, ApacheRequest *req, database *db);

int h_ph_new_request(request_rec *r, session* sid, ApacheRequest *req, database *db, 
		int formaction_success, int formaction_fail);


int h_ph_tag(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES c, 
		int formaction_success, int formaction_fail);

int h_ph_value_key(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES c,
		int formaction_success, int formaction_fail);

int h_sh_admin_product(request_rec *r, session* sid, ApacheRequest *req, database *db);


int h_ph_clear_locks(request_rec *r, char *key, database *db);

#endif
/////////////////////////////////////////////
