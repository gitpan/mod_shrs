/*
 * psql.c
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

#include "psql.h"
#include "util.h"
#include "libpq-fe.h"

static int result_ok(database *db, PGresult *res, char* incode);
static int executed_ok(database *db, PGresult *res);

void set_error_no(database *db, long no) 
{ db->error_no = no; }

void set_error_msg(database *db, char* msg)
{
   db->error_msg = ap_pstrdup(db->pool, msg);
}

char* 	db_error_no(database *db) 
{ return ap_psprintf(db->pool, "%i", db->error_no); }

char* 	db_error_msg(database *db) 
{ return db->error_msg; }

void db_construct(request_rec *r, database *db)
{
   db->pool = r->pool;
   db->server = r->server;
   db->error_no = 0;
   db->error_msg = NULL; 
}

int db_connect(database *db, char* dbname)
{
   db->conn = (PGconn*) PQconnectdb( ap_pstrcat(db->pool, "dbname=", dbname, NULL) );
   set_error_no(db, PQstatus(db->conn)); 
   if ( db->error_no == CONNECTION_BAD) 
   {
      set_error_msg(db, PQerrorMessage(db->conn)); 
      log(PQerrorMessage(db->conn)); 
      return 0;
   }
   return 1;
}

void db_close_connection(database *db) 
{ PQfinish(db->conn); }


static int result_ok(database *db, PGresult *res, char* incode)
{
   char* ptr=NULL;
   db->error_no = PQresultStatus(res);
   if ( db->error_no != PGRES_TUPLES_OK )
   {
      ptr =  PQresultErrorMessage(res); 
      set_error_msg(db, ptr);
      log( ap_pstrcat(db->pool, incode, ": ", ptr, NULL) ); 
      return 0;
   }  
   return 1;
}  

static int executed_ok(database *db, PGresult *res)
{
   char* ptr;
   db->error_no = PQresultStatus(res);
   if ( db->error_no != PGRES_COMMAND_OK )
   {
      ptr =  PQresultErrorMessage(res); 
      log(PQresultErrorMessage(res)); 
      return 0;
   }  
   return 1;
}  

int execute_sql(database *db, char* sql)
{
   int status=0;
   PGresult *res;
   if(!sql) return 0;
   res = PQexec(db->conn, sql); 
   status = executed_ok(db, res );
   PQclear(res);
   return status; 
}

twodim* recordset(database *db, char* sql, int column_dimension)
{
   int row, col, row_cnt, col_cnt;
   int status;
   PGresult *res;
   twodim *td=NULL;
   char* buf=NULL;

   res = PQexec(db->conn, sql); 

   if (!result_ok(db, res, "recordset") ) 
   {
      PQclear(res);
      return NULL;
   }  
 
   row_cnt = PQntuples(res);
   col_cnt = PQnfields(res);
   if (column_dimension < col_cnt) 
	column_dimension = col_cnt;

   if ( row_cnt == 0 )
      set_error_msg(db, "No records found");

   if ( row_cnt > 0 ) 
   {  
      td = td_create_twodim(db->pool);
      if ( td_set_dimensions( db->pool, td, row_cnt, column_dimension ) )  
      {
         for (row=0; row < row_cnt; row++) 
            for( col=0; col < col_cnt; col++) 
               td_set_val( db->pool, td, row, col, trim(db->pool, PQgetvalue(res, row, col)));
      }
   }
   PQclear(res);
   return td; 
}

char* fetch_str(database *db, char* sql)
{
   int row_cnt;
   PGresult *res;
   char *buf=NULL;
 
   res = PQexec(db->conn, sql); 

   if ( result_ok(db, res, "fetch_str") ) 
   {
      row_cnt = PQntuples(res);
      if ( row_cnt == 1 ) 
         buf = trim(db->pool, PQgetvalue(res, 0, 0));
      else if ( row_cnt == 0 )
         set_error_msg(db, "No records found");
      else if ( row_cnt > 1 ) 
        log("Multiple records found during fetch_str");
   }

   PQclear(res);
   return buf; 
}
