/*
 * form_request.h
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

#ifndef _FORM_REQUEST_H
#define _FORM_REQUEST_H

#include "httpd.h"
#include "apache_request.h"
#include "shrs.h"
#include "db_shrs.h"
#include "form.h"

int h_sh_new_request_page1(request_rec *, session* sid, database *db);
int h_sh_new_request_page2(request_rec *, session* sid, ApacheRequest *req, database *db); 
int h_sh_new_request_page3(request_rec *, session* sid, ApacheRequest *req, database *db); 

int h_sh_record_request(request_rec *r, session* sid, char* requested_oid); 
int h_sh_request(request_rec *r, session* sid, ApacheRequest *req, database* db);

int h_ph_save_request(request_rec *r, session* sid, ApacheRequest *req, database *db);
void h_ph_checkin_request(ApacheRequest *req, session* sid, database *db);

static listbox* create_feature(request_rec *r, int status, user_type *ut);
static listbox* create_feature(request_rec *r, int status, user_type *ut);
static listbox* create_components(request_rec *r, int status, user_type *ut);
static listbox* create_comp_types(request_rec *r, int status, user_type *ut);
static listbox* create_products(request_rec *r, int status, user_type *ut);
static listbox* create_assigned_to(request_rec *r, int status, user_type *ut);
static listbox* create_verified_by(request_rec *r, int status, user_type *ut);
static listbox* create_platforms(request_rec *r, int status, user_type *ut);
static listbox* create_originators(request_rec *r, int status, user_type *ut);
static listbox* create_resolutions(request_rec *r, int status, user_type *ut);
static listbox* create_request_types(request_rec *r, int status, user_type *ut);
static listbox* create_priorities(request_rec *r, int status, user_type *ut);
static listbox* create_severities(request_rec *r, int status, user_type *ut);
static listbox* create_reproducibility(request_rec *r, int status, user_type *ut);
static listbox* create_finding_modes(request_rec *r, int status, user_type *ut);
static listbox* create_affects_docs(request_rec *r, int status, user_type *ut);
static listbox* create_marketing_revs(request_rec *r, int status, user_type *ut);
static listbox* create_pam_releases(request_rec *r, int status, user_type *ut);
static listbox* create_fixed_in_comp_revs(request_rec *r, int status, user_type *ut);
static listbox* create_comp_revs(request_rec *r, int status, user_type *ut);
static listbox* create_finding_activities(request_rec *r, int status, user_type *ut);

static textbox* create_level_of_effort(request_rec *r, int status, user_type *ut);
static textbox* create_release_date(request_rec *r, int status, user_type *ut);

static textarea* create_desc_request(request_rec *r, int status, user_type *ut);
static textarea* create_desc_resolution(request_rec *r, int status, user_type *ut);
static textarea* create_desc_verification(request_rec *r, int status, user_type *ut);
#endif
