/*
 * twodim.c
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

#include "twodim.h"

void td_set(ap_pool *pool, twodim *td)
{
   if (td) {
      td->r = 1;
      td->c = 1;
      td->a = (char**) ap_palloc(pool, sizeof(char*) );
      if(td->a) td->a[0]=NULL;
   }
}

twodim* td_create_twodim(ap_pool *pool)
{
   twodim *td;
   td  = (twodim*) ap_palloc(pool, sizeof(twodim) );
   td_set(pool, td); 
   return td; 
}

int td_set_dimensions(ap_pool *pool, twodim *td, int rows, int cols)
{
   int i, size;

   if (!td)
      td = td_create_twodim(pool);

   size = rows*cols;
   if (size<=0) return 0;

   td->r = rows;
   td->c = cols;
   td->a = (char**) ap_palloc(pool, sizeof(char*)*size);
   if(!td->a) return 0;

   for(i=0;i<size;i++)
   {
      td->a[i] = ap_palloc(pool, sizeof(char));      
      if(!td->a[i]) return 0;
      td->a[i][0]=0;
   }
   return 1;
}

int td_row_count(twodim *td) { return td->r; }

int td_col_count(twodim *td) { return td->c; }

char* td_get_val(twodim *td, int row, int col )
{
   if (!td || !td->a ) return NULL;

   if ( row < 0 || col < 0 || row >= td->r || col >= td->c ) return NULL;

   return td->a[row*td->c + col];
}

int td_set_val(ap_pool *pool, twodim *td, int row, int col, char* str )
{
   if (!td || !td->a ) return 0;

   if ( row < 0 || col < 0 || row >= td->r || col >= td->c ) return 0;
 
   if (str) {
      td->a[ row*td->c + col]  = (char*) ap_pstrdup(pool, str);
   }
   return 1;
}

twodim* td_set_test(ap_pool *pool)
{
   twodim* td = 0;
   td = td_create_twodim(pool); 
   td_set_dimensions(pool, td, 3, 3);
   td_set_val(pool, td, 0, 0, "a");
   td_set_val(pool, td, 0, 1, "b yourself");
   td_set_val(pool, td, 0, 2, "c");
   td_set_val(pool, td, 1, 0, "d");
   td_set_val(pool, td, 1, 1, "e coli");
   td_set_val(pool, td, 1, 2, "f");
   td_set_val(pool, td, 2, 0, "g");
   td_set_val(pool, td, 2, 1, "h elp");
   td_set_val(pool, td, 2, 2, "i");
   return td;
}
