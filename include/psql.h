/*
 * psql.h
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

#ifndef _PSQL_H
#define _PSQL_H

#include "httpd.h"
#include "http_log.h"
#include "twodim.h"

#define log(msg)  ap_log_error(APLOG_MARK, APLOG_DEBUG, db->server, msg) 

typedef struct {
   ap_pool 	*pool; 
   server_rec 	*server;
   void 	*conn;
   long 	error_no;
   char 	*error_msg;
} database; 

void 	db_set_error_no(database *db, long no);
void 	db_set_error_msg(database *db, char* msg);

char* 	db_error_no(database *db); 
char* 	db_error_msg(database *db);

void 	db_construct(request_rec *r, database *db);
int     db_connect(database *db, char* dbname);
void 	db_close_connection(database *db);

int 	execute_sql(database *db, char* sql);
twodim* recordset(database *db, char* sql, int column_dimension);
char* 	fetch_str(database *db, char* sql);

//////////////////////////////////////////////////////////////////////////
#endif // _PSQL_H
