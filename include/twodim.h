/*
 *
 * twodim.h
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

#ifndef _TWODIM_H
#define _TWODIM_H

#include "httpd.h"

typedef struct {
   int r;
   int c;
   char** a;
} twodim;

twodim* td_create_twodim(ap_pool *pool);
int     td_set_dimensions(ap_pool *pool, twodim *td, int rows, int cols);
char*	td_get_val(twodim *td, int row, int col);
int	td_set_val(ap_pool *pool, twodim *td, int row, int col, char* str);
int 	td_row_count(twodim *td);
int 	td_col_count(twodim *td);

twodim* td_set_test(ap_pool* pool);

#endif
