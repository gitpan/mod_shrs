/*
 * form_menus.h
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

#ifndef _FORM_MENUS_H
#define _FORM_MENUS_H

#include "httpd.h"
#include "form.h"
#include "db_shrs.h"
#include "apache_request.h"

// #include "http_config.h"
// #include "http_core.h"
// #include "http_log.h"
// #include "http_main.h"
// #include "http_protocol.h"
// #include "util_script.h"

int h_sh_login(request_rec *r);

int h_sh_mainmenu(request_rec* r, session* sid); 

int h_sh_admin(request_rec *r, session* sid);

int h_sh_add_or_edit(request_rec *r, session* sid, ApacheRequest *req, const char* title, CLASSES class, 
   	  		int add_menuaction, int edit_menuaction, int cancel_formaction );

int h_sh_select_to_continue(request_rec *r, session* sid, database *db, CLASSES class, 
		const char* title, int continue_formaction, int cancel_formaction);

int h_ph_insert(request_rec *r, session* sid, database *db, CLASSES class, 
		ApacheRequest req, int continue_formaction); 

int h_ph_update(request_rec *r, session* sid, database *db, CLASSES class, 
		ApacheRequest req, int continue_formaction); 

// int h_sh_add_or_continue(request_rec *r, session* sid, database *db, CLASSES class, 
//		const char* title, int add_menuaction, int continue_menuaction, int cancel_formaction );

#endif
////////////////////////////////////////
