/*
 * form_menus.c
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

#include "form_menus.h"


int h_sh_login(request_rec *r) {

   r->content_type = "text/html";
   ap_send_http_header(r);

   ap_rputs("<html><head>", r);
   ap_rputs("<meta http-equiv=\"pragma\" content=\"no-cache\" >", r);
   ap_rputs("<meta http-equiv=\"expires\" content=\"0\" >", r);
   ap_rputs("<link REL='StyleSheet' type='text/css' HREF='style1.css'>", r);

   ap_rputs("<script language='Javascript'>", r);

   ap_rputs("function Verify() {", r);
   ap_rputs("  with(document.loginform) {", r);
   ap_rputs("    if ( login.value == '' )  {", r);
   ap_rputs("      alert('Please, enter your login name.');", r);
   ap_rputs("      return false;", r);
   ap_rputs("    }", r);

   ap_rputs("    if ( password.value == '' )  {", r);
   ap_rputs("      alert('Please, enter your password.');", r);
   ap_rputs("      return false;", r);
   ap_rputs("    }", r);
   ap_rputs("  }", r);
   ap_rputs("  return true;", r);
   ap_rputs("}", r);

   ap_rputs("</script></head>", r);
   ap_rputs("<body onLoad=\"document.loginform.login.focus();\" >", r);
   ap_rputs("<center>", r);

   ap_rputs("<br><br>", r);
   ap_rputs("<a href=/index.html >Home</a>", r);
   ap_rputs("<br><br>", r);

   ap_rputs("<form name='loginform' action='/shrs' method='post' onsubmit='return Verify();'>", r);

   ap_rputs("<table class=login align='center'>", r);
   ap_rputs("<tr><td align=left ><img src='site.bmp'></td></tr>", r);

   ap_rputs("<tr><td>&nbsp;", r);

   ap_rputs("</td></tr><tr>", r);
   ap_rputs("<td><table class='loginform' > ", r);
   ap_rputs("<tr><td class=header1 colspan='2' align=center >", r);
   ap_rputs("Software/Hardware Request System", r);
   ap_rputs("</td></tr>", r);

   ap_rputs("<tr><td>&nbsp;</td></tr>", r);

   ap_rputs("<tr>", r);
   ap_rputs("<td align='left' width='30%'>User name</td>", r);
   ap_rputs("<td width='50%'><input class=a type='text' name='login' size='25'></td>", r);
   ap_rputs("</tr>", r);

   ap_rputs("<tr>", r);
   ap_rputs("<td align='left' width='30%' >Password</td>", r);
   ap_rputs("<td><input class=a type='password' name='password' size='25'></td>", r);
   ap_rputs("</tr>", r);

   ap_rputs("<tr><td>&nbsp;</td><td>", r);
   ap_rputs("<input type='submit' name='login' VALUE='Login'>", r);
   ap_rputs("<input type='reset'  name='reset' VALUE='Reset'>", r);
   ap_rputs("</td></tr>", r);

   ap_rputs("<tr><td>&nbsp;</td></tr>", r);
   ap_rputs("</table></td></tr>", r);

   ap_rputs("<tr><td>&nbsp;</td></tr>", r);
   ap_rputs("<tr>", r);
   ap_rputs("<td class=copyright >", r);
   ap_rputs("Property of John Atkins <br>Copyright &copy; All rights reserved.<br>", r);
   ap_rputs("</td></TR>", r);
   ap_rputs("</table>", r);
   ap_rputs("</form></center></body></html>", r);
   return OK;
}


int h_sh_mainmenu(request_rec* r, session* sid) 
{
   sh_start(r);
   sh_form_shrs(r, "The Software Hardware Request System", 96, 0, NULL, "center");
   sh_formaction_submit_button(r, 1, 1, "Submit", A_NEW_REQUEST_PAGE_1);
   sh_formaction_submit_button(r, 2, 1, "Process", A_RECORD_REQUEST);
   sh_formaction_submit_button(r, 3, 1, "Summary", A_SUMMARY);
   sh_formaction_submit_button(r, 4, 1, "Reports", A_REPORT);
   sh_formaction_submit_button(r, 5, 1, "Admin", A_ADMIN);
   sh_formaction_submit_button(r, 6, 1, "Password", A_CHG_PWD);
   sh_formaction_submit_button(r, 7, 1, "Logout", A_LOGOUT);
   sh_row_space(r);

   sh_end_shrs(r, sid->key, 0, 0);
   return OK;
}

int h_sh_admin(request_rec *r, session* sid)
{
   sh_start(r);

   sh_form_shrs(r, "Administration", 84, 0, NULL,  "right");

   sh_button(r, 1, 1, "Close", "document.forms['cancel'].submit();");
   sh_row_space(r);
   sh_fcell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "System", A_SYS_USERS);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Components",  A_COMP);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Products", A_PRODUCT);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Platforms",  A_PLATFORM);

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Users",  A_USER);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Comp Types",  A_SELECT_COMP_TYPE);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "PAM Releases", A_SELECT_PAM_RELEASE);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Request Types",  A_REQUEST_TYPE);

   sh_ncell(r, 3);
   ap_rputs("&nbsp;", r);
  
   sh_next_row(r);
   sh_fcell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Originators",  A_ORIGINATOR);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Comp Revisions",  A_SELECT_COMP_REV);

   sh_ncell(r, 1);
   sh_formaction_submit_button(r, 0, 0 , "Marketing Revs", A_SELECT_MARKETING_REV);

   sh_ncell(r, 3);
   sh_formaction_submit_button(r, 0, 0 , "Resolutions",  A_RESOLUTION);

   sh_next_row(r);
   sh_fcell(r, 1);
   ap_rputs("&nbsp;", r);

   sh_ncell(r, 1);
   ap_rputs("&nbsp;", r);

   sh_ncell(r, 1);
   ap_rputs("&nbsp;", r);

   sh_ncell(r, 3);
   sh_formaction_submit_button(r, 0, 0, "Finding Activities",  A_FINDING_ACT);

   sh_row_div(r, "");

   sh_end_shrs(r, sid->key, 0, A_MAIN);
   return OK;
}


int h_sh_add_or_edit(request_rec *r, session* sid, ApacheRequest* req, const char* title, 
CLASSES class, int add_menuaction, int edit_menuaction, int cancel_formaction )
{
   int status;
   char *onadd;
   char *onedit;
   char *oid;

   oid = (char*)ApacheRequest_param(req, "fkey");
   if (!oid) oid = (char*)ApacheRequest_param(req, "oid");

   // Set Form Action  
   onadd = ap_psprintf(r->pool, 
		"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", 
		add_menuaction);

   onedit = ap_psprintf(r->pool, 
		"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", 
		edit_menuaction); 

   sh_start(r);
   sh_form_shrs(r, title, 84, 0, NULL, "right");
   sh_button(r, 2, 1, "Edit", onedit);
   sh_button(r, 1, 1, "Add", onadd);

   // Most forms have a cancel button.
   sh_button(r, 1, 1, "Close", "document.forms['cancel'].submit();");

   if (oid) sh_hidden_str(r, "oid", oid );
   sh_end_shrs(r, sid->key, edit_menuaction, cancel_formaction);
   return OK;
}

int h_sh_select_to_continue(request_rec *r, session* sid, database *db, CLASSES class, const char* title, int continue_formaction, int cancel_formaction)
{
   int status;
   char *oncontinue;
   listbox s;

   set_listbox(r, &s, "oid", 3, 0, 0, NULL);

   oncontinue = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", 
		continue_formaction);

   s.list = db_fetch_list(db, class, NULL);
   if (!s.list)
       return sh_message_return(r, sid->key, "Database", db_error_msg(db), A_MAIN );

   set_selection_listbox(r->pool, &s, "1");

   sh_start(r);
   sh_form_shrs(r, title, 84, 0, NULL, "right");
   sh_button(r, 2, 1, "Continue", oncontinue);
   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();");
   sh_row_space(r);
   sh_fcell(r, 6);
   ap_rputs("&nbsp;Make a selection to continue:&nbsp;&nbsp;", r);
   sh_listbox(r, &s);
   sh_hidden_val(r, "class", class );
   sh_end_shrs(r, sid->key, continue_formaction, cancel_formaction);
   return OK;
}

//int h_sh_add_or_continue(request_rec *r, session* sid, database *db, CLASSES class, 
//		const char* title, int add_menuaction, int continue_menuaction, int cancel_formaction )
//{
//   int status;
//   char *onadd;
//   char *oncontinue;
//   listbox s;
//
//   set_listbox(r, &s, "oid", 3, 0, 0, NULL);
//
//   s.list = db_fetch(db, 0, NULL);
//   if (!s.list)
//       return sh_message_return(r, sid, "Database", db_error_msg(db), A_MAIN);
//
//
//   // Set Form Action  
//   onadd = ap_psprintf(r->pool, 
//		"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", 
//		add_menuaction);
//
//   oncontinue = ap_psprintf(r->pool, 
//		"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", 
//		continue_menuaction); 
//
//   // Start HTML
//   sh_start(r);
//   sh_form_shrs(r, title, 84, 0, NULL, "right");
//   sh_button(r, 2, 1, "Continue", oncontinue);
//   sh_button(r, 1, 1, "Add", onadd);
//   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();");
//
//   sh_row_space(r);
//   sh_fcell_c(r, 6);
//   ap_rputs("&nbsp;Make a selection to continue or add a new record.", r);
//   sh_row_space(r);
//   sh_fcell_c(r, 6);
//
//   // Print Select
//   // request_rec, select
//   sh_listbox(r, &s);
//   sh_hidden_val(r, "class", class );
//   sh_hidden_val(r, "menuaction", 0 );
//   sh_end_shrs(r, sid->key, continue_menuaction, cancel_formaction);
//   return OK;
//}
