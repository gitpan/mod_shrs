/*
 * form_summary.h
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

#ifndef _FORM_SUMMARY_H 
#define _FORM_SUMMARY_H

#include "httpd.h"
#include "apache_request.h"
#include "shrs.h"
#include "db_shrs.h"


int h_sh_summary(request_rec *r, session* sid, ApacheRequest *req,  database* db);


#endif
///////////////////////////////////
