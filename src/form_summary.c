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

#include "form_summary.h"

static void sh_summary(request_rec *r, summary* ptr);

int h_sh_summary(request_rec *r, session* sid, ApacheRequest *req,  database* db)
{
   int i;
   int status;

   summaries* p1=NULL;
   summary* p2;

   p1 = (summaries*) db_fetch(db, CL_SUMMARIES, "");
   if (!p1) return sh_message_return(r, sid->key, "Out of Order", "", A_MAIN);

   sh_start(r);

   sh_form_shrs(r, "Summary", 84, 0, NULL, "right");
   sh_button(r, 1, 1, "Close", "document.forms['cancel'].submit();");
   sh_row_space(r);
   for(i=0;i<p1->size;i++)
   {
     p2 = p1->pp[i];
     if(p2) sh_summary(r, p2);
   }
   sh_hidden_val(r, "requested_oid", 0); 
   sh_end_shrs(r, sid->key, A_REQUEST, A_MAIN);
   return OK;
}


static void sh_summary(request_rec *r, summary* ptr)
{
   char* onopen;
   onopen = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].requested_oid.value=%s; document.forms['shrs'].submit(); }\"", ptr->oid); 

   sh_fcell(r,6);
   sh_button(r, 1, 0, "Open", onopen); 
   ap_rprintf(r, "&nbsp;Record Number %s", ptr->oid);

   sh_next_row(r);
   sh_fcell(r,1);
   ap_rputs("<u>Status</u>", r);
   sh_ncell(r,1);
   ap_rputs("<u>Severity</u>", r);
   sh_ncell(r,1);
   ap_rputs("<u>Priority</u>", r);
   sh_ncell(r,1);
   ap_rputs("<u>Assigned To</u>", r);
   sh_ncell(r,1);
   ap_rputs("<u>Compoonent</u>", r);
   sh_ncell(r,1);
   ap_rputs("<u>Revision</u>", r);

   sh_next_row(r);
   sh_fcell(r,1);
   ap_rprintf(r, "%s", ptr->status );
   sh_ncell(r,1);
   ap_rprintf(r, "%s", ptr->severity );
   sh_ncell(r,1);
   ap_rprintf(r, "%s", ptr->priority );
   sh_ncell(r,1);
   ap_rprintf(r, "%s", ptr->assigned_to );
   sh_ncell(r,1);
   ap_rprintf(r, "%s", ptr->component );
   sh_ncell(r,1);
   ap_rprintf(r, "%s", ptr->revision );

   sh_row_space(r);
   sh_fcell(r,6);
   ap_rputs("<u>Title</u>", r);
   
   sh_next_row(r);
   sh_fcell(r,6);
   ap_rprintf(r, "%s", ptr->title );

   sh_row_space(r);
   sh_fcell(r,6);
   ap_rputs("<u>Description of New Feature or Defect</u>", r);
   
   sh_next_row(r);
   sh_fcell(r,6);
   ap_rprintf(r, "%s", ptr->request );
   sh_row_div(r, "");

}
