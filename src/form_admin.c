/*
 * form_admin.c
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
#include "apache_request.h"
#include "form.h"
#include "db_shrs.h"


////////////////////////////////////
// components 
//    component_oid
//    component	
//    comp_type_oid
//    comp_mgr_oid
// component_revisions 
//    comp_rev_oid
//    comp_rev
//    component_oid
// pam_releases
//    pam_release_oid
//    pam_release
//    product_oid
// marketing_revisions
//    marketing_rev_oid
//    marketing_rev
//    product_oid
// status_history 
//    request_oid
//    user_oid
//    status
//    ts

// "Admin" main page 
// >[Admin Product]
// Admin Products [Edit][Add][Cancel]
//    >[Edit]
//    Select a product to continue [oid:Listbox][Continue][Cancel]
//       >Select and [Continue]
//       Edit Product (admin_product)  [value:TextBox] [oid:hidden] [Marketing Revisions][Pam Releases][Save][Close]
//             >[Marketing Revision] 
//             Admin Marketing Revisions (edit_or_add) [if oid:hidden] [Edit][Add][Cancel]
//                >[Edit]                        
//                Edit Marketing Revision (edit_lookup) [listbox][value:textbox][Save][Cancel
//                >[Add]
//                Add Marketing Revision (add_lookup) [if fkey:hidden] [value:textbox][Save][Cancel
//                   >[Save] 
//                   Success[OK]
//                     >[OK]
//                     Admin Marketing Revisions (edit or add)



// The following function creates a textbox for entring a new item and
// lists the current items already entered.
// All independent lookup tables use this routine directly from main.
// The class param is passed to the function that inserts the data

int h_sh_add_lookup(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES c, 
  	char* label, int next_formaction, int cancel_formaction, int maxlength) 
{
   int i;
   char* fkey=NULL;
   char* oncancel;
   int status;
   textbox t;
   twodim *td=NULL;

   fkey = (char*)ApacheRequest_param(req, "oid");
 
   td =  db_fetch_list(db, c, fkey);
   if (!td)
      return sh_message_return(r, sid->key, "add lookup fetch list failed", "", A_ADMIN);

   // Textbox parms: request_rec, texbox, name of textbox parm, tabindex, maxlength, size.
   set_textbox(r, &t, "value", 3, 0, maxlength, NULL);

   oncancel= ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", cancel_formaction ); 
   sh_start(r);

   sh_form_shrs(r, ap_pstrcat(r->pool, "Add ", label, NULL), 84, 0, NULL, "right");
   sh_button(r, 2, 1, "Save", "submit()" );
   sh_button(r, 1, 1, "Cancel", oncancel); 
   sh_row_space(r);

   sh_fcell_c(r, 6);
   sh_textbox(r, &t);

   sh_row_div(r, ap_pstrcat(r->pool, "Current ", label, "s", NULL ));

   sh_fcell(r, 6);

   // List collection
   for(i=0;i<td_row_count(td);i++) 
      ap_rprintf(r, "&nbsp;&nbsp;&nbsp;%s <BR>\n", td_get_val(td, i, 1) );

   sh_row_space(r);
   sh_fcell(r, 6);

   if (fkey) sh_hidden_str(r, "fkey", fkey); 

   sh_end_shrs(r, sid->key, next_formaction, 0);
   return OK;
}

// This function offers a lookup selection of items 
// that may be renamed in the textbox that follows it. 
int h_sh_edit_lookup(request_rec *r, session* sid, ApacheRequest *req, database *db, 
	CLASSES c, char* label, int next_formaction, int cancel_formaction, int maxlength) 
{
   int i;
   char* fkey;
   char* oncancel;
   int status;
   listbox s;
   textbox t;

   fkey = (char*)ApacheRequest_param(req, "oid");

   set_listbox(r, &s, "oid", 3, 0, 0, NULL);
   set_textbox(r, &t, "value", 4, 0, maxlength, NULL);

   s.list = db_fetch_list(db, c, fkey);  
   if (!s.list)
      return sh_message_return(r, sid->key, "ERROR", db_error_msg(db), A_MAIN);

   set_selection_listbox(r->pool, &s, "1");

   oncancel= ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", cancel_formaction ); 
   sh_start(r);

   sh_form_shrs(r, ap_pstrcat(r->pool, "Edit ", label, NULL), 84, 0, NULL, "right");
   sh_button(r, 1, 1, "Save", "document.forms['shrs'].submit();" );
   sh_button(r, 2, 1, "Cancel",  oncancel);
   sh_row_space(r);

   sh_fcell_c(r, 6);
   ap_rputs("&nbsp;&nbsp;Select an item&nbsp;&nbsp;", r);
   sh_listbox(r, &s);
   ap_rputs("&nbsp;&nbsp;and enter a new value:&nbsp;&nbsp;", r);
   sh_textbox(r, &t);

   if (fkey) sh_hidden_str(r, "fkey", fkey); 

   sh_end_shrs(r, sid->key, next_formaction, 0);
   return OK;
}


int h_sh_admin_user(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   int status;

   user *usr=NULL;

   hidden 	oid;
   textbox	account;
   textbox	last_name;
   textbox	first_name;
   textbox	initials;
   textbox	email;
   checkbox	is_active;
   textbox	password;
   textbox	verify_pwd;

   set_hidden(&oid, "oid", NULL);

   set_textbox(r, &account, "account", 0, 0, MAX_USER_ACCOUNT, NULL);
   set_textbox(r, &first_name,  "first_name" , 0, 0, MAX_FIRST_NAME, NULL);
   set_textbox(r, &last_name,  "last_name", 0, 0, MAX_LAST_NAME, NULL);
   set_textbox(r, &initials, "initials", 0, 0, MAX_INITIALS, NULL);
   set_textbox(r, &email, "email", 0, 0, MAX_EMAIL, NULL);

   set_checkbox(r, &is_active,  "is_active", 0, "Account Active");

   set_textbox(r, &password, "password", 0, 0, MAX_PASSWORD, NULL);

   set_textbox(r, &verify_pwd, "verifiy_pwd", 0, 0, MAX_PASSWORD, NULL);

   oid.value = (char*)ApacheRequest_param(req, "oid");

   if (oid.value) {
 
      usr = db_fetch(db, CL_USER, oid.value);
      if (!usr)
           return sh_message_return(r, sid->key, "ERROR", db_error_msg(db), A_ADMIN);

      account.value = usr->account;
      last_name.value = usr->last_name;
      first_name.value = usr->first_name;
      initials.value = usr->initials;
      email.value = usr->email_address;
      if (usr->is_active) check_checkbox(&is_active);
      password.value = "";
      verify_pwd.value = ""; 
   }
        

   ////////////////////////////// 
   sh_start(r);

   if (oid.value)
      sh_form_shrs(r, "Edit User", 84, 0, NULL, "right");
   else 
      sh_form_shrs(r, "Add New User", 84, 0, NULL, "right");
 
   sh_button(r, 2, 1, "Save", "document.forms['shrs'].submit();" );
   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();" );
   sh_row_space(r);

   sh_fcell(r, 1);
   ap_rputs("Account Name", r);
   sh_ncell(r, 3);
   sh_textbox(r, &account);

   sh_ncell(r, 1);
   sh_checkbox(r, &is_active);

   sh_row_space(r);

   sh_fcell(r, 1);
   ap_rputs("First Name", r);

   sh_ncell(r, 2);
   sh_textbox(r, &first_name);

   sh_ncell(r, 1);
   ap_rputs("Last Name", r);

   sh_ncell(r, 2);
   sh_textbox(r, &last_name);

   sh_row_space(r);
   
   sh_fcell(r, 1);
   ap_rputs("Initials", r);

   sh_ncell(r, 2);
   sh_textbox(r, &initials);

   sh_ncell(r, 1);
   ap_rputs("Email Address", r);

   sh_ncell(r, 2);
   sh_textbox(r, &email);

   if (!oid.value) {

      sh_row_space(r);
      sh_fcell(r, 1);
      ap_rputs("Password", r);
   
      sh_ncell(r, 2);
      sh_password(r, &password);
   
      sh_fcell(r, 1);
      ap_rputs("Verirfy Password", r);
   
      sh_ncell(r, 2);
      sh_password(r, &verify_pwd);
   }

   sh_row_div(r, "");

   sh_ncell(r, 1);
   if (oid.value) sh_hidden(r, &oid); 

   sh_end_shrs(r, sid->key, A_SAVE_USER, A_USER );
   return OK;
}


int h_sh_admin_system(request_rec *r, session* sid, database *db)
{
   char* onclear=NULL;
   listbox  qa_mgr;
   listbox  market_mgr;
   listbox  comp_super;
   system_users  *ptr;

   ptr = db_fetch(db, CL_SYS_USERS, "");
   if(!ptr) 
      return sh_outoforder(r, "Failed to fetch sys users in sys admin");

   set_listbox(r, &qa_mgr, "qa_mgr_oid", 10, 0, 0, NULL);
   set_listbox(r, &comp_super, "comp_super_oid", 11, 0, 0, NULL);
   set_listbox(r, &market_mgr, "market_mgr_oid", 12, 0, 0, NULL);
  
   qa_mgr.list = db_fetch_list(db, CL_USER, NULL);  
   market_mgr.list = db_fetch_list(db, CL_USER, NULL);  
   comp_super.list = db_fetch_list(db, CL_USER, NULL);  

   set_selection_listbox(r->pool, &qa_mgr, ptr->qa_mgr_oid);
   set_selection_listbox(r->pool, &comp_super, ptr->comp_super_oid);
   set_selection_listbox(r->pool, &market_mgr, ptr->market_mgr_oid);

   onclear = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", A_CLEAR_ALL_LOCKS); 

   //////////////////////////////
   sh_start(r);
   
   sh_form_shrs(r, "System", 80, 0, NULL, "right");

   sh_button(r, 1, 4, "Clear All Request Locks", onclear);
   sh_button(r, 1, 3, "Save", "document.forms['shrs'].submit();");
   sh_button(r, 0, 1, "Cancel", "document.forms['cancel'].submit();");
   sh_row_space(r);
   sh_fcell(r, 2);
   ap_rputs("&nbsp;QA Manager", r);  
   sh_ncell(r, 2);
   sh_listbox(r, &qa_mgr);
   sh_ncell(r, 2);
   ap_rputs("&nbsp;", r);

   sh_next_row(r);
   sh_fcell(r, 2);
   ap_rputs("&nbsp;Component Supervisor", r);  
   sh_ncell(r, 2);
   sh_listbox(r, &comp_super);
   sh_ncell(r, 2);
   ap_rputs("&nbsp;", r);

   sh_next_row(r);
   sh_fcell(r, 2);
   ap_rputs("&nbsp;Marketing Manager", r);  
   sh_ncell(r, 2);
   sh_listbox(r, &market_mgr);
   sh_ncell(r, 2);
   ap_rputs("&nbsp;", r);

   sh_end_shrs(r, sid->key, A_SAVE_SYS_USERS, A_ADMIN);
   return OK;
}


int h_ph_save_system_users(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   system_users  su;

   su.qa_mgr_oid = (char*) ApacheRequest_param(req, "qa_mgr_oid" );
   su.comp_super_oid = (char*) ApacheRequest_param(req, "comp_super_oid" );
   su.market_mgr_oid = (char*) ApacheRequest_param(req, "market_mgr_oid" );

   if ( db_save(db, CL_SYS_USERS, &su) )
      return sh_message_return(r, sid->key, "Success", "", A_ADMIN);
   else
      return sh_message_return(r, sid->key, "Failed", "", A_ADMIN);
}


int h_sh_admin_comp(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   int 		status;
   char		*title;
   char		*onRevisions;
   char		*onCompTypes;
   hidden 	oid;
   textbox 	comp_name;
   listbox  	comp_type;
   listbox  	comp_mgr;
   component    *comp;

   set_hidden(&oid, "oid", NULL);
   set_listbox(r, &comp_type, "comp_type_oid", 4, 0, 0, NULL);
   set_listbox(r, &comp_mgr, "comp_mgr_oid", 5, 0, 0, NULL);
   set_textbox(r, &comp_name, "name", 3, 0, MAX_COMPONENT, NULL);

   comp_mgr.list = db_fetch_list(db, CL_USER, NULL);  
   if(!comp_mgr.list)
      return sh_message_return(r, sid->key, "ERROR", db_error_msg(db), A_COMP);
   comp_type.list = db_fetch_list(db, CL_COMP_TYPE, NULL);  
   if(!comp_type.list)
      return sh_message_return(r, sid->key, "ERROR", db_error_msg(db), A_COMP);

   oid.value = (char*)ApacheRequest_param(req, "oid");

   if (oid.value)  // then a component was selected for edit
   {
     title=ap_pstrdup(r->pool, "Edit Component");
     comp = db_fetch(db, CL_COMPONENT, oid.value);
     if (!comp) 
        return sh_message_return(r, sid->key, "ERROR", db_error_msg(db), A_COMP);
     
     comp_name.value = comp->name;
     set_selection_listbox(r->pool, &comp_type, comp->comp_type_oid);
     set_selection_listbox(r->pool, &comp_mgr, comp->user_oid );

   } else {
     title=ap_pstrdup(r->pool, "Add New Componet");
     set_selection_listbox(r->pool, &comp_type, "1");
     set_selection_listbox(r->pool, &comp_mgr, "1");
   }

   onRevisions = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", A_COMP_REV); 

   onCompTypes = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", A_COMP_TYPE); 
   
   //////////////////////////////
   sh_start(r);
   
   sh_form_shrs(r, title, 84, 0, NULL, "right");
   if (oid.value) {
      sh_button(r, 0, 1, "Revisions", onRevisions); 
      sh_button(r, 0, 2, "Component Types", onCompTypes); 
   }
   sh_button(r, 2, 3, "Save", "document.forms['shrs'].submit();");
   sh_button(r, 0, 1, "Cancel", "document.forms['cancel'].submit();");
   sh_row_space(r);

   sh_fcell(r, 1);
   ap_rputs("&nbsp;Name", r);  

   sh_ncell(r, 2);
   sh_textbox(r, &comp_name);

   sh_ncell(r, 1);
   ap_rputs("Component Type", r);

   sh_ncell(r, 2);
   sh_listbox(r, &comp_type);

   sh_row_space(r);

   sh_fcell(r, 3);
   sh_nbsp(r, 1);

   sh_ncell(r, 1);
   ap_rputs("Component Manager", r);

   sh_ncell(r, 2);
   sh_listbox(r, &comp_mgr); 

   if (oid.value)  sh_hidden(r, &oid);

   sh_end_shrs(r, sid->key, A_SAVE_COMP, A_COMP);
   return OK;
}


int h_sh_admin_product(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   int 		status;
   char		*title;
   char		*onMarketRev;
   char		*onPamRev;
   hidden 	oid;
   textbox 	product;

   set_hidden(&oid, "oid", NULL);
   set_textbox(r, &product, "value", 5, 0, MAX_PRODUCT, NULL);

   oid.value = (char*)ApacheRequest_param(req, "oid");

   if (oid.value)  
   {
     title=ap_pstrdup(r->pool, "Edit Product");
     product.value = db_fetch(db, CL_PRODUCT, oid.value);
     if (!product.value) 
        return sh_message_return(r, sid->key, "ERROR", db_error_msg(db), A_PRODUCT);

   } else {
     title=ap_pstrdup(r->pool, "Add New Product ");
   }

   onMarketRev = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", A_MARKETING_REV); 

   onPamRev = ap_psprintf(r->pool, 
	"\"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }\"", A_PAM_RELEASE); 
   
   //////////////////////////////
   sh_start(r);
   
   sh_form_shrs(r, title, 84, 0, NULL, "right");
   if (oid.value) {
      sh_button(r, 0, 1, "Market Revisions", onMarketRev); 
      sh_button(r, 0, 2, "PAM Releases", onPamRev); 
   }
   sh_button(r, 2, 3, "Save", "document.forms['shrs'].submit();");
   sh_button(r, 0, 5, "Cancel", "document.forms['cancel'].submit();");

   sh_row_space(r);

   // name
   sh_fcell(r, 6 );
   ap_rputs("&nbsp;Name&nbsp;&nbsp;", r);  
   sh_textbox(r, &product);


   if (oid.value)  sh_hidden(r, &oid);

   sh_end_shrs(r, sid->key, A_SAVE_PRODUCT, A_PRODUCT);
   return OK;
}

int h_ph_save_component(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   component comp;
   comp.oid		= (char*) ApacheRequest_param(req, "oid" );
   comp.name		= (char*) ApacheRequest_param(req, "name" );
   comp.comp_type_oid	= (char*) ApacheRequest_param(req, "comp_type_oid" );
   comp.user_oid  	= (char*) ApacheRequest_param(req, "comp_mgr_oid" );

   if ( db_save(db, CL_COMPONENT, &comp) )
      return sh_message_return(r, sid->key, "Success", "", A_COMP);
   else
      return sh_message_return(r, sid->key, "Failed", "", A_COMP);
}


int h_ph_save_user(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   user usr;

   usr.oid			=  (char*) ApacheRequest_param(req, "oid" );
   usr.account			=  (char*) ApacheRequest_param(req, "account" );
   usr.password			=  (char*) ApacheRequest_param(req, "password" );
   usr.last_name		=  (char*) ApacheRequest_param(req, "last_name" );
   usr.first_name		=  (char*) ApacheRequest_param(req, "first_name" );
   usr.initials			=  (char*) ApacheRequest_param(req, "initials" );
   usr.email_address		=  (char*) ApacheRequest_param(req, "email" );

   if ( ApacheRequest_param(req, "is_qa_mgr" ) )
      usr.is_qa_mgr = 1;
   else
      usr.is_qa_mgr = 0;

   if ( ApacheRequest_param(req, "is_comp_sup" ) )
      usr.is_comp_sup	= 1;
   else
      usr.is_comp_sup	= 0;

   if ( ApacheRequest_param(req, "is_market_mgr" ) )
      usr.is_market_mgr = 1;
   else
      usr.is_market_mgr 	= 0;

   if ( ApacheRequest_param(req, "is_active" ) )
      usr.is_active	= 1;
   else
      usr.is_active	= 0;

//   if ( ApacheRequest_param(req, "is_tracking" ) )
//      usr.is_tracking	 = 1;
//   else
//      usr.is_tracking	 = 0;


   if ( db_save(db, CL_USER, &usr) )
      return sh_message_return(r, sid->key, "Success", "", A_USER);
   else
      return sh_message_return(r, sid->key, "Failed", db_error_msg(db), A_USER);
}

int h_ph_new_request(request_rec *r, session* sid, ApacheRequest *req, database *db, 
		int formaction_success, int formaction_fail)
{
   new_request new_req;

   new_req.oid			= (char*)ApacheRequest_param(req, "oid" );
   new_req.title		= (char*)ApacheRequest_param(req, "title" );
   new_req.feature_oid		= (char*)ApacheRequest_param(req, "feature_oid" );
   new_req.submitter_oid	= (char*)ApacheRequest_param(req, "submitter_oid" );
   new_req.finding_act_oid	= (char*)ApacheRequest_param(req, "finding_act_oid" );
   new_req.platform_oid		= (char*)ApacheRequest_param(req, "platform_oid" );
   new_req.originator_oid	= (char*)ApacheRequest_param(req, "originator_oid" );
   new_req.request_type_oid	= (char*)ApacheRequest_param(req, "request_type_oid" );
   new_req.product_oid		= (char*)ApacheRequest_param(req, "product_oid	" );
   new_req.component_oid	= (char*)ApacheRequest_param(req, "component_oid" );
   new_req.comp_rev_oid			= (char*)ApacheRequest_param(req, "comp_rev_oid" );
   new_req.severity_oid			= (char*)ApacheRequest_param(req, "severity_oid" );
   new_req.reproducibility_oid	= (char*)ApacheRequest_param(req, "reproducibility_oid" );
   new_req.finding_mode_oid	= (char*)ApacheRequest_param(req, "finding_mode_oid" );
   new_req.affects_doc_oid	= (char*)ApacheRequest_param(req, "affects_doc_oid" );
   new_req.request		= (char*)ApacheRequest_param(req, "request" );

   if (db_save(db, CL_NEW_REQUEST, &new_req))
      return sh_message_return(r, sid->key, "Success", "", A_ADMIN);
   else
      return sh_message_return(r, sid->key, "Failed", db_error_msg(db), A_ADMIN);
}


int h_ph_tag(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES c, 
		int formaction_success, int formaction_fail)
{
   int status=0;
   tag tagged;
   tagged.oid = NULL;
   tagged.value = NULL;

   tagged.oid	= (char*)ApacheRequest_param(req, "oid"); 
   tagged.value	= (char*)ApacheRequest_param(req, "value"); 

   if (!tagged.value) 
      return sh_message_return(r, sid->key, "Tag Failed", tagged.value, formaction_fail); 

   status  = db_save(db, c, &tagged); 
   if ( status )
      return sh_message_return(r, sid->key, "Saved", "", formaction_success);
   else
      return sh_message_return(r, sid->key, "Tag Failed", ap_psprintf(r->pool, "%i", status) , formaction_fail); 
}

int h_ph_value_key(request_rec *r, session* sid, ApacheRequest *req, database *db, CLASSES c,
		int formaction_success, int formaction_fail)
{
   new_value_key vk;

   vk.oid	= (char*)ApacheRequest_param(req, "oid" );
   vk.value	= (char*)ApacheRequest_param(req, "value" );
   vk.fkey	= (char*)ApacheRequest_param(req, "fkey" ); 

   if (!vk.value) 
      // return sh_outoforder(r, "form_admin, ph_value_key, missing vk.value"); 
      return sh_message_return(r, sid->key, "Failed", "A value must be entered", formaction_fail);

   if (!vk.fkey) 
      return sh_message_return(r, sid->key, "Out of Order", "", formaction_fail);

   if (db_save(db, c, &vk))
   {
      hidden h;
      h.name = "fkey";
      h.value = ap_pstrdup(r->pool, vk.fkey); 
      return sh_message_return_hidden(r, sid->key, "Saved", "", formaction_success, &h);
   }
   else
      return sh_message_return(r, sid->key, "Failed", db_error_msg(db), formaction_fail);
}


int h_ph_clear_locks(request_rec *r, char *key, database *db)
{
   db_delete_all_request_locks(db);
   return sh_message_return(r, key, "Done", "", A_SYS_USERS);
}



///////////////////////////////////////////
//   switch (c) {
//
//   case CL_REQUEST:
//      break;
//
//   case CL_USER:
//      break;
//
//   case CL_SESSION:
//      break;
//
//   case CL_COMPONENT:
//      break;
//
//   case CL_COMP_TYPE:
//      break;
//
//   case CL_COMP_REV:
//      break;
//
//   case CL_PRODUCT:
//      break;
//
//   case CL_PAM:
//      break;
//
//   case CL_MARKET_REV:
//      break;
//
//   case CL_FINDING_ACT:
//      break;
//
//   case CL_ORIG:
//      break;
//
//   case CL_PLATFORM:
//      break;
//
//   case CL_RESOLUTION:
//      break;
//
//   case CL_REQUEST_TYPE:
//      break;
//
//   case CL_COMMENT:
//      break;
//
//   case CL_STATUS_HISTORY:
//      break;
//
