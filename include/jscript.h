/*
 * jscript.h
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

#ifndef _JSCRIPT_H
#define _JSCRIPT_H

#include "httpd.h"

void sh_js_help(request_rec *r);
void sh_js_cancel(request_rec *r, int formaction);
void sh_js_validate_string1(request_rec *r);
void sh_js_validate_record_number(request_rec *r);

#endif
