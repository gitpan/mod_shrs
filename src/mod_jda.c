/*
 * mod_jda.c
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

module MODULE_VAR_EXPORT jda_module;

int shrs_handler(request_rec *r );

static handler_rec jda_handlers[] =
{
    { "shrs-handler", shrs_handler},
    {NULL}
};

module MODULE_VAR_EXPORT jda_module =
{
    STANDARD_MODULE_STUFF,
    NULL,               	// module initializer                 
    NULL,		// per-directory config creator       
    NULL, 		// dir config merger  
    NULL,               // server config creator              
    NULL,               // server config merger               
    NULL,        	// command table 
    jda_handlers,     	// [7]  content handlers              
    NULL,               // [2]  URI-to-filename translation   
    NULL,               // [5]  check/validate user_id        
    NULL,               // [6]  check user_id is valid *here* 
    NULL,               // [4]  check access by host address  
    NULL,               // [7]  MIME type checker/setter      
    NULL,               // [8]  fixups                        
    NULL,               // [9]  logger                        
    NULL,               // [3]  header parser                 
    NULL,               // process initialization             
    NULL,               // process exit/cleanup               
    NULL                // [1]  post read_request handling    
};




