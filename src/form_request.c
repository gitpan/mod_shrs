
/*
 * form_request.c
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

#include "form_request.h"
#include "jscript.h"

#define SENDMAIL_CMD "/usr/lib/sendmail"
#define SENDMAIL_ARGS "-oi", "-t"

static void send_email(request_rec* r, twodim *td, char* msg);

static int view_a_request_update(request_rec *r, request_update* ptr);
static void select_email_finalists(email_canidates *pec, int starting_status, int new_status);

static void sh_js_validate_request(request_rec *r) 
{
   // Validate submit form
 
   ap_rputs( "var bModified = false;\n\n", r );

   ap_rputs("static void onFormChanged()\n{\n", r );
   ap_rputs("   bModified = true;\n", r );
   ap_rputs("   return;\n}\n\n", r );
   
   sh_js_help(r);
   ap_rputs("static void component_oid_onchange()\n{\n", r);
   ap_rputs("   document.submit_form.action='submit';\n", r);
   ap_rputs("   document.submit_form.submit();\n}\n\n", r);

   ap_rputs("static void validate_submit()\n{\n", r);
   ap_rputs("   var buf='The following item(s) ", r); 
   ap_rputs("need to be completed:\\n';\n", r);
   ap_rputs("   var bOK=true;\n\n", r);
   ap_rputs("   with ( document.submit_form ) {\n\n", r);
   ap_rputs("      if (  affects_doc_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Affects Documentation\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( component_type_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Component Type\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( component_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Component\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( component_revision_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Component Revision\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( finding_activity_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Finding Activity\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( finding_mode_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Finding Mode\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( platform_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Platform\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( product_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Product ID\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( reproducibility_oid.value < 2 ) {\n", r);
   ap_rputs("        buf = buf  +  '   Reproducibility\\n';\n", r);
   ap_rputs("        bOK = false;\n       }\n", r);
   ap_rputs("      if ( severity_oid.value < 2 ) {\n", r);
   ap_rputs("         buf = buf  +  '   Severity\\n';\n", r);
   ap_rputs("      if (( request_title.value == null ) \n", r);
   ap_rputs("		|| ( request_title.value == '' )) {\n", r);
   ap_rputs("         buf = buf  +  '   Title\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n", r);
   ap_rputs("      if ( ( description.value == null ) \n", r);
   ap_rputs("		|| ( description.value == '' ) ) {\n", r);
   ap_rputs("         buf = buf  +  '   Description of ", r);
   ap_rputs("Modification\\n';\n", r);
   ap_rputs("         bOK = false;\n      }\n   }\n", r);
   ap_rputs("      if ( !bOK  ) alert(buf);\n", r);
   ap_rputs("         return bOK;\n      }\n\n", r);

   ap_rputs("static void submit_click()\n{\n", r);
   ap_rputs("   if ( validate_submit() )\n", r); 

   ap_rputs("   document.submit_form.submit();\n}\n\n", r);
   
   ap_rputs("static void logout_click()\n{\n", r);
   ap_rputs("   if ( bModified ) { }\n}\n", r);
} 


int h_sh_new_request_page1(request_rec *r, session* sid, database *db)
{
   const int width_of_listbox=0;
   listbox	components;
   listbox	products;

   set_listbox(r, &components, 	"component_oid", 1, width_of_listbox, 0, NULL);
   set_listbox(r, &products, 	"product_oid", 2, width_of_listbox, 0, NULL);

   components.list = db_fetch_list(db, CL_COMPONENT, NULL);
   if (!components.list)
      return sh_message_return(r, sid->key, "Failed to fetch component list", "", A_MAIN);

   products.list = db_fetch_list(db, CL_PRODUCT, NULL);
   if (!components.list)
      return sh_message_return(r, sid->key, "Failed to fetch product list", "", A_MAIN);

   set_selection_listbox(r->pool, &components, "1");
   set_selection_listbox(r->pool, &products, "1");

   // Start HTML
   sh_start(r);
   sh_js_help(r);
   sh_form_shrs(r, "Submit New Request, page 1", 84, 0, NULL, "right");
   sh_button(r, 2, 3, "Continue", "document.forms['shrs'].submit();" );
   sh_button(r, 1, 4, "Cancel", "document.forms['cancel'].submit();");

   sh_row_space(r);

   sh_fcell(r, 1);
   sh_a_help(r, 1, "Product", "product");

   sh_ncell(r, 2);
   sh_listbox(r, &products);

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component", "component");

   sh_ncell(r, 2);
   sh_listbox(r, &components);

   sh_row_div(r, "");

   sh_end_shrs(r, sid->key, A_NEW_REQUEST_PAGE_2, A_MAIN);
   return OK;
}


int h_sh_new_request_page2(request_rec *r, session* sid, ApacheRequest *req, database *db) 
{
   int	width_of_listbox=0;
   char*	product=NULL;
   component    *comp;
   hidden	component_oid;
   hidden	product_oid;

   listbox	comp_revs;
   listbox	pam_releases;

   component_oid.name = ap_pstrdup(r->pool, "component_oid");
   product_oid.name = ap_pstrdup(r->pool, "product_oid");

   component_oid.value = (char*) ApacheRequest_param(req, "component_oid" );
   if (!component_oid.value)
      return sh_message_return(r, sid->key, "Failed to obtain component_oid", "", A_MAIN);

   product_oid.value = (char*) ApacheRequest_param(req, "product_oid" );
   if (!product_oid.value)
      return sh_message_return(r, sid->key, "Failed to obtain product_oid", "", A_MAIN);

   comp = (component*) db_fetch(db, CL_COMPONENT, component_oid.value); 
   if (!comp)
      return sh_message_return(r, sid->key, "Failed to fetch component", "", A_MAIN);

   product = (char*) db_fetch(db, CL_PRODUCT, product_oid.value); 
   if (!product)
      return sh_message_return(r, sid->key, "Failed to fetch product name", "", A_MAIN);

   set_listbox(r, &comp_revs, "comp_rev_oid", 1, width_of_listbox, 0, NULL);
   set_listbox(r, &pam_releases, "pam_release_oid", 2, width_of_listbox, 0, NULL);

   comp_revs.list = db_fetch_list(db, CL_COMP_REV, component_oid.value);
   if (!comp_revs.list)
      return sh_message_return(r, sid->key, "Failed to fetch comp_revs list", "", A_MAIN);

   pam_releases.list = db_fetch_list(db, CL_PAM, product_oid.value );
   if (!comp_revs.list)
      return sh_message_return(r, sid->key, "Failed to fetch product list", "", A_MAIN);

   set_selection_listbox(r->pool, &comp_revs, "1");
   set_selection_listbox(r->pool, &pam_releases, "1");

   // Start HTML
   sh_start(r);
   sh_js_help(r);
   sh_form_shrs(r, "Submit New Request, page 2", 84, 0, NULL, "right");
   sh_button(r, 2, 3, "Continue", "document.forms['shrs'].submit();" );
   sh_button(r, 1, 4, "Cancel", "document.forms['cancel'].submit();");

   sh_row_space(r);

   sh_fcell(r, 1);
   sh_a_help(r, 1, "Product", "product");

   sh_ncell(r, 2);
   ap_rprintf(r, "%s", product); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component", "component");

   sh_ncell(r, 2);
   ap_rprintf(r, "%s", comp->name); 

   sh_row_space(r);

   sh_fcell(r, 1);
   sh_a_help(r, 1, "PAM Release", "pamrelease");

   sh_ncell(r, 2);
   sh_listbox(r, &pam_releases);

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component Revision", "componentrev");

   sh_ncell(r, 2);
   sh_listbox(r, &comp_revs);

   sh_row_div(r, "");
   sh_hidden(r, &component_oid);
   sh_hidden(r, &product_oid);


   sh_end_shrs(r, sid->key, A_NEW_REQUEST_PAGE_3, A_MAIN);
   return OK;
}

int h_sh_new_request_page3(request_rec *r, session* sid, ApacheRequest *req, database *db) 
{
   const int width_of_listbox=35;
   textbox	title;
   listbox	feature;
   listbox	finding_activities;
   listbox	platforms;
   listbox	originators;
   listbox	resolutions;
   listbox	request_types;
   listbox	severities; 
   listbox	reproducibility;
   listbox	finding_modes;
   listbox	affects_docs;
   listbox	marketing_revs;
   textarea	desc;

   component    *comp;

   char*        comp_type;
   char*        comp_rev;
   char*	product;
   char*	pam_release;

   hidden       component_oid;
   hidden       comp_rev_oid;
   hidden       product_oid;
   hidden       pam_release_oid;

   // Define controls:
   set_listbox(r, &feature, 		"feature_oid", 		1, width_of_listbox, 0, NULL);
   set_listbox(r, &finding_activities, 	"finding_act_oid",	2, width_of_listbox, 0, NULL);
   set_listbox(r, &platforms, 		"platform_oid", 	3, width_of_listbox, 0, NULL);
   set_listbox(r, &originators, 	"originator_oid", 	4, width_of_listbox, 0, NULL);
   set_listbox(r, &resolutions, 	"resolution_oid", 	5, width_of_listbox, 0, NULL);
   set_listbox(r, &request_types, 	"request_type_oid", 	6, width_of_listbox, 0, NULL);
   set_listbox(r, &severities, 		"severity_oid", 	7, width_of_listbox, 0, NULL);
   set_listbox(r, &reproducibility, 	"reproducibility_oid", 	8, width_of_listbox, 0, NULL);
   set_listbox(r, &finding_modes, 	"finding_mode_oid", 	9, width_of_listbox, 0, NULL);
   set_listbox(r, &affects_docs, 	"affects_doc_oid",	10, width_of_listbox, 0, NULL);
   set_listbox(r, &marketing_revs, 	"market_rev_oid", 	11, width_of_listbox, 0, NULL);
      
   set_textbox(r, &title, "title", 12, 80, MAX_TITLE, NULL);

   set_textarea(r, &desc, "request", 13, 10, 60, MAX_REQUEST, NULL);

   component_oid.name = "component_oid";
   product_oid.name = "product_oid";
   comp_rev_oid.name = "comp_rev_oid";
   pam_release_oid.name = "pam_release_oid";

   // Fetch form parameters from apache request.
   component_oid.value = (char*) ApacheRequest_param(req, "component_oid" );
   if (!component_oid.value)
      return sh_message_return(r, sid->key, "Failed to obtain component_oid", "", A_MAIN);
  
   product_oid.value = (char*) ApacheRequest_param(req, "product_oid" );
   if (!product_oid.value)
      return sh_message_return(r, sid->key, "Failed to obtain product_oid", "", A_MAIN);

   comp_rev_oid.value = (char*) ApacheRequest_param(req, "comp_rev_oid" );
   if (!comp_rev_oid.value)
      return sh_message_return(r, sid->key, "Failed to obtain comp_rev_oid", "", A_MAIN);

   pam_release_oid.value = (char*) ApacheRequest_param(req, "pam_release_oid" );
   if (!pam_release_oid.value) 
      return sh_message_return(r, sid->key, "Failed to obtain pam_release_oid", "", A_MAIN);

   // Fetch form parameters from database 
   comp = (component*) db_fetch(db, CL_COMPONENT, component_oid.value); 
   if (!comp)
      return sh_message_return(r, sid->key, "Failed to fetch component", "", A_MAIN);

   // .. names
   comp_type = db_fetch(db, CL_COMP_TYPE, comp->comp_type_oid); 
   if (!comp_type)
      return sh_message_return(r, sid->key, "Failed to fetch comp_type name", "", A_MAIN);

   comp_rev = db_fetch(db, CL_COMP_REV, comp_rev_oid.value); 
   if (!comp_rev)
      return sh_message_return(r, sid->key, "Failed to fetch comp_rev name", "", A_MAIN);

   product = db_fetch(db, CL_PRODUCT, product_oid.value); 
   if (!product)
      return sh_message_return(r, sid->key, "Failed to fetch product name", "", A_MAIN);

   pam_release = db_fetch(db, CL_PAM, pam_release_oid.value); 
   if (!pam_release)
      return sh_message_return(r, sid->key, "Failed to fetch pam_release name", "", A_MAIN);

   // ..data for listboxes
   feature.list = db_fetch_list(db, CL_FEATURE, NULL);
   if (!feature.list)
      return sh_message_return(r, sid->key, "Failed to fetch feature list", "", A_MAIN);

   finding_activities.list = db_fetch_list(db, CL_FINDING_ACT, NULL);
   if (!finding_activities.list )
      return sh_message_return(r, sid->key, "Failed to fetch finding_activities list", "", A_MAIN);

   platforms.list = db_fetch_list(db, CL_PLATFORM, NULL);
   if (!platforms.list) 
      return sh_message_return(r, sid->key, "Failed to fetch platforms list", "", A_MAIN);

   originators.list = db_fetch_list(db, CL_ORIG, NULL);
   if (!originators.list) 
      return sh_message_return(r, sid->key, "Failed to fetch originators list", "", A_MAIN);

   resolutions.list = db_fetch_list(db, CL_RESOLUTION, NULL);
   if (!resolutions.list) 
      return sh_message_return(r, sid->key, "Failed to fetch resolutions list", "", A_MAIN);

   request_types.list = db_fetch_list(db, CL_REQUEST_TYPE, NULL);
   if (!request_types.list) 
      return sh_message_return(r, sid->key, "Failed to fetch request_types list", "", A_MAIN);

   severities.list = db_fetch_list(db, CL_SEVERITY, NULL); 
   if (!severities.list) 
      return sh_message_return(r, sid->key, "Failed to fetch severities list", "", A_MAIN);

   reproducibility.list = db_fetch_list(db, CL_REPRODUCIBILITY, NULL);
   if (!reproducibility.list) 
      return sh_message_return(r, sid->key, "Failed to fetch reproducibility list", "", A_MAIN);

   finding_modes.list = db_fetch_list(db, CL_FINDING_MODE, NULL);
   if (!finding_modes.list) 
      return sh_message_return(r, sid->key, "Failed to fetch finding_modes list", "", A_MAIN);

   affects_docs.list = db_fetch_list(db, CL_AFFECTS_DOCS, NULL);
   if (!affects_docs.list) 
      return sh_message_return(r, sid->key, "Failed to fetch affects_docs list", "", A_MAIN);

   marketing_revs.list = db_fetch_list(db, CL_MARKET_REV, product_oid.value );
   if (!marketing_revs.list) 
      return sh_message_return(r, sid->key, "Failed to fetch marketing_revs list", "", A_MAIN);


   // Set default selection for listboxes:
   set_selection_listbox(r->pool, &feature, "1");
   set_selection_listbox(r->pool, &finding_activities, "1");
   set_selection_listbox(r->pool, &platforms, "1");
   set_selection_listbox(r->pool, &originators, "1");
   set_selection_listbox(r->pool, &resolutions, "1");
   set_selection_listbox(r->pool, &request_types, "1");
   set_selection_listbox(r->pool, &severities, "1"); 
   set_selection_listbox(r->pool, &reproducibility, "1");
   set_selection_listbox(r->pool, &finding_modes, "1");
   set_selection_listbox(r->pool, &affects_docs, "1");
   set_selection_listbox(r->pool, &marketing_revs, "1");

   ////// 
   sh_start(r);
   sh_js_help(r);
   sh_form_shrs(r, "New Request for Modification", 94, 0, NULL , "right");
   sh_button(r, 1, 3, "Save", "document.forms['shrs'].submit();");
   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();");
   sh_row_space(r);

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Defect or<BR>New Feature", "feature");

   sh_ncell(r, 2);
   sh_listbox(r, &feature );

   sh_ncell(r, 3);
   sh_nbsp(r, 1);

   sh_row_div(r, "");
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Severity", "severity");

   sh_ncell(r, 2);
   sh_listbox(r, &severities);

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Platform", "platform");

   sh_ncell(r, 2);
   sh_listbox(r, &platforms );

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Originator", "originator");

   sh_ncell(r, 2);
   sh_listbox(r, &originators );

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Product", "product");

   sh_ncell(r, 2);
   ap_rprintf(r, "&nbsp;%s", product);
 
   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Reproducibility", "reproducibility");

   sh_ncell(r, 2);
   sh_listbox(r, &reproducibility );

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Marketing Revision", "marketingrev");

   sh_ncell(r, 2);
   sh_listbox(r, &marketing_revs );

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Type of Request", "typeofrequest");

   sh_ncell(r, 2);
   sh_listbox(r, &request_types );

   sh_ncell(r, 3);
   sh_nbsp(r, 1);

   sh_row_div(r, "");
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Finding Activity", "findingactivity");

   sh_ncell(r, 3);
   sh_listbox(r, &finding_activities );

   sh_ncell(r, 2);
   sh_nbsp(r, 1); 

   sh_next_row(r); 
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Finding Mode", "findingmode");

   sh_ncell(r, 2);
   sh_listbox(r, &finding_modes );

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component Type", "componenttype");

   sh_ncell(r, 2);
   if(comp_type)  ap_rprintf(r, "&nbsp;%s", comp_type); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Affects<BR>Documentation", "affectsdocs");

   sh_ncell(r, 2);
   sh_listbox(r, &affects_docs );

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component", "component");

   sh_ncell(r, 2);
   ap_rprintf(r, "&nbsp;%s", comp->name); 

   sh_next_row(r);
   sh_fcell(r, 3);
   sh_nbsp(r, 1);

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component Rev", "componentrev");

   sh_ncell(r, 2);
   if(comp_rev) ap_rprintf(r, "&nbsp;%s", comp_rev); 

   sh_next_row(r); 
   sh_fcell(r, 3);
   ap_rputs("&nbsp;", r); 

   sh_row_div(r, "");
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Title", "title");

   sh_ncell(r, 5);
   sh_textbox(r, &title);

   sh_row_space(r);

   sh_next_row(r);
   sh_fcell_v(r, 1);
   sh_a_help(r, 1, "Description of:</BR>Defect or <BR>New Feature,<BR>How To Reproduce,<BR>Test Environment,<BR>Implication,<BR>Suggested Fix,<BR>Attachments", "desc");

   sh_ncell(r, 5);
   sh_textarea(r, &desc); 

   sh_hidden(r, &component_oid );
   sh_hidden(r, &comp_rev_oid );
   sh_hidden(r, &product_oid );
   sh_hidden(r, &pam_release_oid );

   sh_end_shrs(r, sid->key, A_SAVE_NEW_REQUEST, A_MAIN ); 
   return OK;
}


int h_sh_record_request(request_rec *r, session* sid, char* requested_oid) 
{
   textbox record_no;

   set_textbox(r, &record_no, "requested_oid", 1, 0, MAX_RECORD_NO, NULL);

   sh_start(r);
   sh_js_help(r);
   // sh_js_validate_string1(r);
   // sh_js_validate_record_number(r);
   
   sh_form_shrs(r, "Record Request", 60, 0, NULL , "right");
   sh_button(r, 2, 1, "Open", "document.forms['shrs'].submit();" );
   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();" );
   sh_row_space(r);

   sh_fcell_c(r, 6);
   ap_rputs("&nbsp;Enter a Record Number:&nbsp;", r);
   sh_textbox(r, &record_no);

   if(requested_oid)
   {
      sh_row_space(r);
      sh_next_row(r);
      sh_fcell_c(r, 6);
      ap_rprintf(r, "&nbsp;Record %s was not found.:&nbsp;", requested_oid);
   }
   sh_end_shrs(r, sid->key, A_REQUEST, A_MAIN );
   return OK;
}


int h_ph_save_new_request(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
    char* buf=NULL;
    new_request newreq;

    newreq.title		 = (char*) ApacheRequest_param(req, "title" );
    newreq.feature_oid		 = (char*) ApacheRequest_param(req, "feature_oid" );
    newreq.submitter_oid	 = ap_pstrdup(r->pool, sid->user_oid); 
    newreq.finding_act_oid	 = (char*) ApacheRequest_param(req, "finding_act_oid" );
    newreq.platform_oid		 = (char*) ApacheRequest_param(req, "platform_oid" );
    newreq.originator_oid	 = (char*) ApacheRequest_param(req, "originator_oid" );
    newreq.request_type_oid	 = (char*) ApacheRequest_param(req, "request_type_oid" );
    newreq.product_oid		 = (char*) ApacheRequest_param(req, "product_oid" );
    newreq.component_oid	 = (char*) ApacheRequest_param(req, "component_oid" );
    newreq.comp_rev_oid		 = (char*) ApacheRequest_param(req, "comp_rev_oid" );
    newreq.severity_oid		 = (char*) ApacheRequest_param(req, "severity_oid" );
    newreq.reproducibility_oid	 = (char*) ApacheRequest_param(req, "reproducibility_oid" );
    newreq.finding_mode_oid	 = (char*) ApacheRequest_param(req, "finding_mode_oid" );
    newreq.affects_doc_oid	 = (char*) ApacheRequest_param(req, "affects_doc_oid" );
    newreq.request		 = (char*) ApacheRequest_param(req, "request" );

   if ( db_save(db, CL_NEW_REQUEST, &newreq) )
   {
      buf = ap_pstrcat(r->pool, "Saved Record Number ", newreq.oid, NULL); 
      return sh_message_return(r, sid->key, "Success", buf, A_MAIN);
   } else {
      return sh_message_return(r, sid->key, "Failed", "", A_MAIN);
   }
}

int h_ph_save_request(request_rec *r, session* sid, ApacheRequest *req, database *db)
{
   char* msg="Saved";
   checkout c;
   char* starting_status=NULL;
   char* comment;
   request_update  ru;
   int status_changed=0;

   starting_status = (char*) ApacheRequest_param(req, "starting_status");
   ru.request_oid = (char*) ApacheRequest_param(req, "request_oid");
   ru.feature_oid = (char*) ApacheRequest_param(req, "feature_oid");
   ru.request_type_oid = (char*) ApacheRequest_param(req, "request_type_oid");
   ru.assigned_to_oid = (char*) ApacheRequest_param(req, "assigned_to_oid");
   ru.verified_by_oid = (char*) ApacheRequest_param(req, "verified_by_oid");
   ru.status_oid = (char*) ApacheRequest_param(req, "status_oid");
   ru.severity_oid = (char*) ApacheRequest_param(req, "severity_oid");
   ru.priority_oid = (char*) ApacheRequest_param(req, "priority_oid");
   ru.reproducibility_oid = (char*) ApacheRequest_param(req, "reproducibility_oid");
   ru.platform_oid = (char*) ApacheRequest_param(req, "platform_oid");
   ru.product_oid = (char*) ApacheRequest_param(req, "product_oid");
   ru.originator_oid = (char*) ApacheRequest_param(req, "originator_oid");
   ru.component_oid = (char*) ApacheRequest_param(req, "component_oid");
   ru.comp_rev_oid = (char*) ApacheRequest_param(req, "comp_rev_oid");
   ru.fixed_in_comp_rev_oid = (char*) ApacheRequest_param(req, "fixed_in_comp_rev_oid");
   ru.finding_act_oid = (char*) ApacheRequest_param(req, "finding_activity_oid");
   ru.finding_mode_oid = (char*) ApacheRequest_param(req, "finding_mode_oid");
   ru.marketing_rev_oid = (char*) ApacheRequest_param(req, "marketing_rev_oid");
   ru.level_of_effort = (char*) ApacheRequest_param(req, "level_of_effort");
   ru.pam_release_oid = (char*) ApacheRequest_param(req, "pam_release_oid");
   ru.affects_doc_oid = (char*) ApacheRequest_param(req, "affects_doc_oid");
   ru.resolution_oid = (char*) ApacheRequest_param(req, "resolution_oid");
   ru.resolution = (char*) ApacheRequest_param(req, "desc_resolution");
   ru.release_date = (char*) ApacheRequest_param(req, "release_date");
   ru.request = (char*) ApacheRequest_param(req, "desc_request");
   ru.verification = (char*) ApacheRequest_param(req, "desc_verification");
   comment = (char*) ApacheRequest_param(req, "comment");

   c.request_oid = ru.request_oid;
   c.user_oid = sid->user_oid;

   if(!db_extend_checkout(db, &c))
   {
     msg = ap_pstrcat(r->pool, "Your checkout time expired.  The record is currently checked out by ",
		c.lockedby, NULL);
     return sh_message_return(r, sid->key, "ERROR: Failed to save request", msg, A_MAIN);
   }

   if (!db_save(db, CL_REQUEST, &ru) )
      return sh_message_return(r, sid->key, "Result", "ERROR: Failed to save request", A_MAIN);


   status_changed = strcmp(starting_status, ru.status_oid); 
   if(status_changed)
   {
      new_history h;
      h.value = ru.status_oid;
      h.user_oid = sid->user_oid;
      h.request_oid = ru.request_oid;
      if (!db_save(db, CL_STATUS_HISTORY, &h))
         log("Failed to insert status history");
   }

   if (comment)  
   {
      if(strlen(comment)>0)
      {
         new_history h;
         h.value = comment;
         h.request_oid = ru.request_oid;
         h.user_oid = sid->user_oid;
         if (!db_save(db, CL_COMMENT, &h))
             msg = ap_pstrcat(r->pool, "ERROR: The request was saved without your comment.<br /><br />", comment, "<br /><br />", NULL);
      }
   }
   db_checkin(db, ru.request_oid, sid->user_oid);

   // Send Email
   if(status_changed)
   {
      twodim *td;
      email_canidates *ec;

      // Fetch system and request user oids.
      ec = db_fetch(db, CL_EMAIL_CANIDATES, ru.request_oid);  
      if(ec) {
         // Add current user. Used by db_fetch_email_finalists 
         // To prevent the current user from receiving an email.
         ec->session_user_oid = sid->user_oid;
         select_email_finalists(ec, atoi(starting_status), atoi(ru.status_oid)); 
         td = (twodim*) db_fetch_email_finalists(db, ec);
         if(td) send_email(r, td, "");
      }
   } 
   return sh_message_return(r, sid->key, "Result", msg, A_MAIN);

}


static void send_email(request_rec* r, twodim *td, char* msg)
{
   int i;
//   for(i=0;i<td_row_count(td);i++)
//       log(td_get_val(td, i, 0));
//    execl(SENDMAIL_CMD, SENDMAIL_CMD, SENDMAIL_ARGS, NULL);
}


static void select_email_finalists(email_canidates *pec, int starting_status, int new_status)
{
   email_canidates ec;

   ec.qa_mgr_oid = NULL;
   ec.comp_super_oid = NULL;
   ec.comp_mgr_oid = NULL;
   ec.submitter_oid = NULL;
   ec.assigned_to_oid = NULL;
   ec.verified_by_oid = NULL;
   ec.market_mgr_oid = NULL;

   switch (starting_status) {

   case STATUS_NEW: 
      if(new_status == STATUS_ASSIGNED)
      {
         ec.assigned_to_oid = pec->assigned_to_oid;
      }
      else if (new_status == STATUS_WITHDRAWN)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
         ec.submitter_oid = pec->submitter_oid;
      }
      else if (new_status == STATUS_REQUEST_INFO)
      {
         ec.submitter_oid = pec->submitter_oid;  
      }
      else if (new_status == STATUS_DECLINED)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.submitter_oid = pec->submitter_oid;  
      }

   break;

   case STATUS_OPEN: 
      if(new_status == STATUS_DECLINED)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.submitter_oid = pec->submitter_oid;  
      } 
      else if(new_status == STATUS_ASSIGNED) 
      {
         ec.assigned_to_oid = pec->assigned_to_oid;
      } 
      else if(new_status == STATUS_REQUEST_INFO) 
      {
         ec.submitter_oid = pec->submitter_oid;  
      } 

   break;

   case STATUS_ASSIGNED : 
      if(new_status == STATUS_OPEN)
      {
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
         ec.assigned_to_oid = pec->assigned_to_oid;
      }
      else if(new_status == STATUS_FIXED)
      {
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
      }
   break;

   case STATUS_FIXED : 
      if(new_status == STATUS_IN_TEST)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.verified_by_oid = pec->verified_by_oid; 
      }
      else if(new_status == STATUS_ASSIGNED)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid ;
         ec.assigned_to_oid = pec->assigned_to_oid;
      }
   break;

   case STATUS_IN_TEST: 
      if(new_status == STATUS_ASSIGNED)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
      }
      else if(new_status == STATUS_VERIFIED)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
      }

   break;

   case STATUS_VERIFIED:
      if(new_status == STATUS_IN_TEST)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
         ec.assigned_to_oid = pec->assigned_to_oid;
         ec.verified_by_oid = pec->verified_by_oid; 
      }
   break;

   case STATUS_REQUEST_INFO: 
      if(new_status == STATUS_OPEN)
      {
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
      }
   break;

   case STATUS_DECLINED: 
      if(new_status == STATUS_OPEN)
      {
         ec.qa_mgr_oid = pec->qa_mgr_oid;
         ec.comp_super_oid = pec->comp_super_oid;
         ec.comp_mgr_oid = pec->comp_mgr_oid;
      }

   break;

   case STATUS_WITHDRAWN: 
   break;

   case STATUS_CLOSED: 
   break;

   }

   pec->qa_mgr_oid = ec.qa_mgr_oid;
   pec->comp_super_oid = ec.comp_super_oid;
   pec->comp_mgr_oid = ec.comp_mgr_oid;
   pec->submitter_oid = ec.submitter_oid;
   pec->assigned_to_oid = ec.assigned_to_oid;
   pec->verified_by_oid = ec.verified_by_oid;
   pec->market_mgr_oid = ec.market_mgr_oid;

} 



static int view_a_request_update(request_rec *r, request_update* ptr)
{
   r->content_type = "text/html";
   ap_rputs("<HTML><HEAD><TITLE></TITLE></HEAD><BODY>", r);

   ap_rprintf(r, "request_oid  %s<BR>", ptr->request_oid );
   ap_rprintf(r, "feature_oid  %s<BR>", ptr->feature_oid );
   ap_rprintf(r, "request_type_oid  %s<BR>", ptr->request_type_oid );
   ap_rprintf(r, "assigned_to_oid  %s<BR>", ptr->assigned_to_oid );
   ap_rprintf(r, "verified_by_oid  %s<BR>", ptr->verified_by_oid );
   ap_rprintf(r, "status_oid  %s<BR>", ptr->status_oid );
   ap_rprintf(r, "severity_oid  %s<BR>", ptr->severity_oid );
   ap_rprintf(r, "priority_oid  %s<BR>", ptr->priority_oid );
   ap_rprintf(r, "reproducibility_oid  %s<BR>", ptr->reproducibility_oid );
   ap_rprintf(r, "platform_oid  %s<BR>", ptr->platform_oid );
   ap_rprintf(r, "product_oid  %s<BR>", ptr->product_oid );
   ap_rprintf(r, "originator_oid  %s<BR>", ptr->originator_oid );
   ap_rprintf(r, "component_oid  %s<BR>", ptr->component_oid );
   ap_rprintf(r, "comp_rev_oid  %s<BR>", ptr->comp_rev_oid );
   ap_rprintf(r, "fixed_in_comp_rev_oid  %s<BR>", ptr->fixed_in_comp_rev_oid );
   ap_rprintf(r, "finding_act_oid  %s<BR>", ptr->finding_act_oid );
   ap_rprintf(r, "finding_mode_oid  %s<BR>", ptr->finding_mode_oid );
   ap_rprintf(r, "marketing_rev_oid  %s<BR>", ptr->marketing_rev_oid );
   ap_rprintf(r, "level_of_effort  %s<BR>", ptr->level_of_effort );
   ap_rprintf(r, "pam_release_oid  %s<BR>", ptr->pam_release_oid );
   ap_rprintf(r, "affects_doc_oid  %s<BR>", ptr->affects_doc_oid );
   ap_rprintf(r, "resolution_oid  %s<BR>", ptr->resolution_oid );
   ap_rprintf(r, "resolution  %s<BR>", ptr->resolution );
   ap_rprintf(r, "release_date  %s<BR>", ptr->release_date );
   ap_rprintf(r, "request  %s<BR>", ptr->request );
   ap_rprintf(r, "verification  %s<BR>", ptr->verification );
   ap_rputs("</BODY></HTML>", r);
   return OK;
}



view_a_request(request_rec *r, request* ptr)
{ 
   r->content_type = "text/html";
   ap_rputs("<HTML><HEAD><TITLE></TITLE></HEAD><BODY>", r);
   ap_rprintf(r, "date_submitted  %s<BR>", ptr->date_submitted );
   ap_rprintf(r, "date_last_modified  %s<BR>", ptr->date_last_modified );
   ap_rprintf(r, "release_date  %s<BR>", ptr->release_date );
   ap_rprintf(r, "title  %s<BR>", ptr->title );
   ap_rprintf(r, "feature.oid %s<BR>", ptr->feature.oid );
   ap_rprintf(r, "feature.value  %s<BR>", ptr->feature.value );
   ap_rprintf(r, "submitter.oid  %s<BR>", ptr->submitter.oid );
   ap_rprintf(r, "submitter.value  %s<BR>", ptr->submitter.value );
   ap_rprintf(r, "assigned_to.oid  %s<BR>", ptr->assigned_to.oid );
   ap_rprintf(r, "assigned_to.value  %s<BR>", ptr->assigned_to.value );
   ap_rprintf(r, "verified_by.oid  %s<BR>", ptr->verified_by.oid );
   ap_rprintf(r, "verified_by.value  %s<BR>", ptr->verified_by.value );
   ap_rprintf(r, "platform.oid  %s<BR>", ptr->platform.oid );
   ap_rprintf(r, "platform.value  %s<BR>", ptr->platform.value );
   ap_rprintf(r, "originator.oid  %s<BR>", ptr->originator.oid );
   ap_rprintf(r, "originator.value  %s<BR>", ptr->originator.value );
   ap_rprintf(r, "resolution.oid  %s<BR>", ptr->resolution.oid );
   ap_rprintf(r, "resolution.value  %s<BR>", ptr->resolution.value );
   ap_rprintf(r, "request_type.oid  %s<BR>", ptr->request_type.oid );
   ap_rprintf(r, "request_type.value  %s<BR>", ptr->request_type.value );
   ap_rprintf(r, "finding_activity.oid  %s<BR>", ptr->finding_activity.oid );
   ap_rprintf(r, "finding_activity.value  %s<BR>", ptr->finding_activity.value );
   ap_rprintf(r, "product.oid  %s<BR>", ptr->product.oid );
   ap_rprintf(r, "product.value  %s<BR>", ptr->product.value );
   ap_rprintf(r, "pam_release.oid  %s<BR>", ptr->pam_release.oid );
   ap_rprintf(r, "pam_release.value  %s<BR>", ptr->pam_release.value );
   ap_rprintf(r, "market_rev.oid  %s<BR>", ptr->market_rev.oid );
   ap_rprintf(r, "market_rev.value  %s<BR>", ptr->market_rev.value );
   ap_rprintf(r, "component.oid  %s<BR>", ptr->component.oid );
   ap_rprintf(r, "component.value  %s<BR>", ptr->component.value );
   ap_rprintf(r, "comp_type  %s<BR>", ptr->comp_type );
   ap_rprintf(r, "comp_rev.oid  %s<BR>", ptr->comp_rev.oid );
   ap_rprintf(r, "comp_rev.value  %s<BR>", ptr->comp_rev.value );
   ap_rprintf(r, "fixed_in_comp_rev.oid  %s<BR>", ptr->fixed_in_comp_rev.oid );
   ap_rprintf(r, "fixed_in_comp_rev.value  %s<BR>", ptr->fixed_in_comp_rev.value );
   ap_rprintf(r, "status.oid  %s<BR>", ptr->status.oid );
   ap_rprintf(r, "status.value  %s<BR>", ptr->status.value );
   ap_rprintf(r, "priority.oid  %s<BR>", ptr->priority.oid );
   ap_rprintf(r, "priority.value  %s<BR>", ptr->priority.value );
   ap_rprintf(r, "severity.oid  %s<BR>", ptr->severity.oid );
   ap_rprintf(r, "severity.value  %s<BR>", ptr->severity.value );
   ap_rprintf(r, "reproducibility.oid  %s<BR>", ptr->reproducibility.oid );
   ap_rprintf(r, "reproducibility.value  %s<BR>", ptr->reproducibility.value );
   ap_rprintf(r, "finding_mode.oid  %s<BR>", ptr->finding_mode.oid );
   ap_rprintf(r, "finding_mode.value  %s<BR>", ptr->finding_mode.value );
   ap_rprintf(r, "affects_docs.oid  %s<BR>", ptr->affects_docs.oid );
   ap_rprintf(r, "affects_docs.value  %s<BR>", ptr->affects_docs.value );
   ap_rprintf(r, "level_of_effort   %s<BR>", ptr->level_of_effort  );
   ap_rprintf(r, "desc_request   %s<BR>", ptr->desc_request  );
   ap_rprintf(r, "desc_resolution   %s<BR>", ptr->desc_resolution  );
   ap_rprintf(r, "desc_verification  %s<BR>", ptr->desc_verification );
   ap_rprintf(r, "comments  %s<BR>", ptr->notes );
   ap_rprintf(r, "status_history  %s<BR>", ptr->status_history);
   ap_rputs("</BODY></HTML>", r);
   return OK;
}


static listbox* create_status(request_rec *r, int status, user_type *ut)
{
   // if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_feature(request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_components(request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_comp_types (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_products (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_assigned_to (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_verified_by (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_platforms (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_originators(request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_resolutions (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_request_types (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_priorities (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_severities (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_reproducibility (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_finding_modes (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_affects_docs (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_marketing_revs (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_pam_releases	(request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_fixed_in_comp_revs (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_comp_revs (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}

static listbox* create_finding_activities (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_listbox(r);
   return NULL;
}


static textbox* create_level_of_effort (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_textbox(r);
   return NULL;
}

static textbox* create_release_date (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_textbox(r);
   return NULL;
}

static textarea* create_desc_request (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_textarea(r);
   return NULL;
}

static textarea* create_desc_resolution (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_textarea(r);
   return NULL;
}

static textarea* create_desc_verification (request_rec *r, int status, user_type *ut)
{
   if (status == STATUS_CLOSED) return NULL; 
   if(ut->qa_mgr) return create_textarea(r);
   return NULL;
}

int h_sh_request(request_rec *r, session* sid, ApacheRequest *req,  database* db)
{
   const int	width_of_listbox=35;
   int 		i;
   int 		is_checked_out=0;
   char		*onchange=NULL;
   char		*oid=NULL;
   char		*param=NULL;
   single_entry *pse=NULL;
   entries	*pe=NULL;
   request	*record=NULL;
   checkout	*checkedout=NULL;

   listbox	*status=NULL; 
   listbox	*feature = NULL;
   listbox	*assigned_to = NULL;
   listbox	*verified_by = NULL;
   listbox	*finding_activities = NULL;
   listbox	*platforms = NULL;
   listbox	*originators = NULL;
   listbox	*resolutions = NULL;
   listbox	*request_types = NULL; 	
   listbox	*priorities = NULL;
   listbox	*severities = NULL;
   listbox	*reproducibility = NULL;
   listbox	*finding_modes = NULL;
   listbox	*affects_docs = NULL;
   listbox	*marketing_revs = NULL;
   listbox	*products = NULL;
   listbox	*pam_releases = NULL;
   listbox	*components = NULL;
   listbox	*comp_revs = NULL;
   listbox	*fixed_in_comp_revs = NULL;

   textbox	*level_of_effort = NULL;
   textbox	*release_date = NULL;

   textarea	*desc_request = NULL;
   textarea	*desc_resolution = NULL;
   textarea	*desc_verification = NULL;
   textarea	*log_entry=NULL;

   hidden       the_request_oid;
   hidden	starting_status;

   user_type	ut;
   system_users *psu;

   starting_status.name = "starting_status";
   the_request_oid.name = "request_oid";

   // Is this a first reqeust? 
   oid = (char*) ApacheRequest_param(req, "requested_oid" );
   if (oid) 
   {
      the_request_oid.value = oid; 

      // Is there a record for the oid requested?
      record = (request*) db_fetch(db, CL_REQUEST, oid);
      if(!record)
         return h_sh_record_request(r, sid, oid); 

      // Checkout the request.
      checkedout = (checkout*) ap_palloc(r->pool, sizeof(checkout) );
      if (!checkedout) 
         return sh_message_return(r, sid->key, "Out Of Order", "", A_MAIN);

      starting_status.value = ap_pstrdup(db->pool, record->status.oid);

      checkedout->request_oid = record->oid;
      checkedout->user_oid    = sid->user_oid; 

      is_checked_out = db_checkout(db, checkedout); 
//return sh_message_return(r, sid->key, "Out Of Order", ap_psprintf(db->pool, "%i", is_checked_out) , A_MAIN);

   } else { 

      // No. We are reprocessing a request. 
      // Only a checked out request may be reprocessed.
      is_checked_out = 1;
      oid = (char*) ApacheRequest_param(req, "request_oid" );
      if(!oid)
         return sh_message_return(r, sid->key, "Out Of Order", "", A_MAIN);

      the_request_oid.value = oid; 

      record = (request*) db_fetch(db, CL_REQUEST, oid);
      if(!record)
         return sh_message_return(r, sid->key, "Out of Order", "", A_MAIN);

      starting_status.value = (char*) ApacheRequest_param(req, "starting_status");
      if(!starting_status.value)
         return sh_message_return(r, sid->key, "Out Of Order", "", A_MAIN);

      param = (char*) ApacheRequest_param(req, "status_oid");
      if (param) record->status.oid 	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "feature_oid");
      if (param) record->feature.oid  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "assigned_to_oid");
      if (param) record->assigned_to.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "verified_by_oid");
      if (param) record->verified_by.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "finding_activity_oid");
      if (param) record->finding_activity.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "platform_oid");
      if (param) record->platform.oid  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "originator_oid");
      if (param) record->originator.oid 	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "resolution_oid");
      if (param) record->resolution.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "request_type_oid");
      if (param) record->request_type.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "priority_oid");
      if (param) record->priority.oid  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "severity_oid");
      if (param) record->severity.oid  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "reproducibility_oid");
      if (param) record->reproducibility.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "finding_mode_oid");
      if (param) record->finding_mode.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "affects_doc_oid");
      if (param) record->affects_docs.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "marketing_rev_oid");
      if (param) record->market_rev.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "product_oid");
      if (param) record->product.oid  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "pam_release_oid");
      if (param) record->pam_release.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "component_oid");
      if (param) record->component.oid  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "comp_rev_oid");
      if (param) record->comp_rev.oid  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "fixed_in_comp_rev_oid");
      if (param) record->fixed_in_comp_rev.oid  = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "level_of_effort");
      if (param) record->level_of_effort  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "release_date");
      if (param) record->release_date  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "desc_request");
      if (param) record->desc_request  	 	= ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "desc_resolution");
      if (param) record->desc_resolution  	 = ap_pstrdup(r->pool, param );
      param = (char*) ApacheRequest_param(req, "desc_verification");
      if (param) record->desc_verification 	 = ap_pstrdup(r->pool, param );

   }

   psu = (system_users*) db_fetch(db, CL_SYS_USERS, "");
   if (!psu)
      return sh_message_return(r, sid->key, "Out of Order", "", A_MAIN);

   ut.qa_mgr = 0;
   ut.comp_super = 0;
   ut.comp_mgr = 0;
   ut.market_mgr = 0;
   ut.submitter = 0;
   ut.assigned_to = 0;
   ut.verified_by = 0;

   if ( strcmp(sid->user_oid, psu->qa_mgr_oid) == 0) ut.qa_mgr = 1;
   if ( strcmp(sid->user_oid, psu->comp_super_oid) == 0) ut.comp_super = 1;
   if ( strcmp(sid->user_oid, psu->market_mgr_oid) == 0) ut.market_mgr = 1;

   if ( strcmp(sid->user_oid, record->submitter.oid) == 0) ut.submitter = 1;
   if ( strcmp(sid->user_oid, record->assigned_to.oid) == 0 ) ut.assigned_to = 1;	
   if ( strcmp(sid->user_oid, record->verified_by.oid) == 0 )  ut.verified_by = 1;
   if ( strcmp(sid->user_oid, record->comp_mgr.oid) == 0) ut.comp_mgr = 1;

   if (is_checked_out)
   {
      int i_status_oid;
      i_status_oid = atoi(starting_status.value); 
      status = create_status(r, i_status_oid, &ut); 
      feature = create_feature(r, i_status_oid, &ut);
      components = create_components(r, i_status_oid, &ut);
      products = create_products(r, i_status_oid, &ut);
      assigned_to = create_assigned_to(r, i_status_oid, &ut);
      verified_by = create_verified_by(r, i_status_oid, &ut);
      platforms = create_platforms(r, i_status_oid, &ut);
      originators = create_originators(r, i_status_oid, &ut);
      resolutions = create_resolutions(r, i_status_oid, &ut);
      request_types = create_request_types(r, i_status_oid, &ut);
      priorities = create_priorities(r, i_status_oid, &ut);
      severities = create_severities(r, i_status_oid, &ut);
      reproducibility = create_reproducibility(r, i_status_oid, &ut);
      finding_modes = create_finding_modes(r, i_status_oid, &ut);
      affects_docs = create_affects_docs(r, i_status_oid, &ut);
      marketing_revs = create_marketing_revs(r, i_status_oid, &ut);
      pam_releases = create_pam_releases(r, i_status_oid, &ut);
      level_of_effort = create_level_of_effort(r, i_status_oid, &ut);
      release_date =  create_release_date(r, i_status_oid, &ut);
      desc_request = create_desc_request(r, i_status_oid, &ut);
      desc_resolution = create_desc_resolution(r, i_status_oid, &ut);
      desc_verification = create_desc_verification(r, i_status_oid, &ut);
      fixed_in_comp_revs = create_fixed_in_comp_revs(r, i_status_oid, &ut);
      comp_revs = create_comp_revs(r, i_status_oid, &ut);
      finding_activities = create_finding_activities(r, i_status_oid, &ut);
      log_entry = create_textarea(r);
   }

   if(status)
   {
      set_listbox(r, status, "status_oid", 1, width_of_listbox, 0, NULL);
      status->list = db_fetch_status_list(db, &ut, starting_status.value );
      if (!status->list)
         return sh_message_return(r, sid->key, "Failed to fetch status list", "", A_MAIN);
      set_selection_listbox(r->pool, status, record->status.oid );
   }

   if ( feature ) 
   {
      set_listbox(r, feature, "feature_oid", 1, width_of_listbox, 0, NULL);
      feature->list = db_fetch_list(db, CL_FEATURE, NULL);
      if (!feature->list)
         return sh_message_return(r, sid->key, "Failed to fetch feature list", "", A_MAIN);
      set_selection_listbox(r->pool, feature, record->feature.oid );
   }

   if ( components ) 
   {
      onchange = ap_psprintf(r->pool, 
	"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }", A_REQUEST);

      set_listbox(r, components, "component_oid", 1, width_of_listbox, 0, onchange);
      components->list = db_fetch_list(db, CL_COMPONENT, NULL); 
      if (!components->list)
         return sh_message_return(r, sid->key, "Failed to fetch components list", "", A_MAIN);
      set_selection_listbox(r->pool, components, record->component.oid );
   }

   if (products)
   {
      onchange = ap_psprintf(r->pool, 
	"{ document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); }", A_REQUEST);

      set_listbox(r, products, "product_oid", 1, width_of_listbox, 0, onchange);
      products->list = db_fetch_list(db, CL_PRODUCT, NULL); 
      if (!products->list)
         return sh_message_return(r, sid->key, "Failed to fetch product list", "", A_MAIN);
      set_selection_listbox(r->pool, products, record->product.oid );
   }

   if ( assigned_to )
   {
      set_listbox(r, assigned_to, "assigned_to_oid", 1, width_of_listbox, 0, NULL);
      assigned_to->list = db_fetch_list(db, CL_USER, NULL);
      if (!assigned_to->list)
         return sh_message_return(r, sid->key, "Failed to fetch assigned_to list", "", A_MAIN);
      set_selection_listbox(r->pool, assigned_to, record->assigned_to.oid );
   }

   if ( verified_by )
   {
      set_listbox(r, verified_by, "verified_by_oid", 1, width_of_listbox, 0, NULL);
      verified_by->list = db_fetch_list(db, CL_USER, NULL);
      if (!verified_by->list)
         return sh_message_return(r, sid->key, "Failed to fetch verified_by list", "", A_MAIN);
      set_selection_listbox(r->pool, verified_by, record->verified_by.oid );
   }
 
   if ( platforms ) 
   {
      set_listbox(r, platforms, "platform_oid", 3, width_of_listbox, 0, NULL);
      platforms->list = db_fetch_list(db, CL_PLATFORM, NULL);
      if (!platforms->list)
         return sh_message_return(r, sid->key, "Failed to fetch platforms list", "", A_MAIN);
      set_selection_listbox(r->pool, platforms, record->platform.oid );
   }

   if ( originators ) 
   {
      set_listbox(r, originators, "originator_oid", 4, width_of_listbox, 0, NULL);
      originators->list = db_fetch_list(db, CL_ORIG, NULL);
      if (!originators->list)
         return sh_message_return(r, sid->key, "Failed to fetch originators list", "", A_MAIN);
      set_selection_listbox(r->pool, originators, record->originator.oid );
   }

   if ( resolutions ) 
   {
      set_listbox(r, resolutions, "resolution_oid", 5, width_of_listbox, 0, NULL);
      resolutions->list = db_fetch_list(db, CL_RESOLUTION, NULL);
      if (!resolutions->list)
         return sh_message_return(r, sid->key, "Failed to fetch resolutions list", "", A_MAIN);
      set_selection_listbox(r->pool, resolutions, record->resolution.oid );
   }

   if ( request_types ) 
   {
      set_listbox(r, request_types, "request_type_oid", 6, width_of_listbox, 0, NULL);
      request_types->list = db_fetch_list(db, CL_REQUEST_TYPE, NULL);
      if (!request_types->list)
         return sh_message_return(r, sid->key, "Failed to fetch request_types list", "", A_MAIN);
      set_selection_listbox(r->pool, request_types, record->request_type.oid );
   }

   if ( priorities ) 
   {
      set_listbox(r, priorities, "priority_oid", 7, width_of_listbox, 0, NULL);
      priorities->list = db_fetch_list(db, CL_PRIORITY, NULL);
      if (!priorities->list)
         return sh_message_return(r, sid->key, "Failed to fetch priorities list", "", A_MAIN);
      set_selection_listbox(r->pool, priorities, record->priority.oid );
   }

   if ( severities ) 
   {
      set_listbox(r, severities, "severity_oid", 7, width_of_listbox, 0, NULL);
      severities->list = db_fetch_list(db, CL_SEVERITY, NULL);
      if (!severities->list)
         return sh_message_return(r, sid->key, "Failed to fetch severities list", "", A_MAIN);
      set_selection_listbox(r->pool, severities, record->severity.oid );
   }

   if ( reproducibility ) 
   {
      set_listbox(r, reproducibility, "reproducibility_oid", 8, width_of_listbox, 0, NULL);
      reproducibility->list = db_fetch_list(db, CL_REPRODUCIBILITY, NULL);
      if (!reproducibility->list)
         return sh_message_return(r, sid->key, "Failed to fetch reproducibility list", "", A_MAIN);
      set_selection_listbox(r->pool, reproducibility, record->reproducibility.oid );
   }

   if ( finding_modes ) 
   {
      set_listbox(r, finding_modes, "finding_mode_oid", 9, width_of_listbox, 0, NULL);
      finding_modes->list = db_fetch_list(db, CL_FINDING_MODE, NULL);
      if (!finding_modes->list)
         return sh_message_return(r, sid->key, "Failed to fetch finding_modes list", "", A_MAIN);
      set_selection_listbox(r->pool, finding_modes, record->finding_mode.oid );
   }

   if ( affects_docs ) 
   {
      set_listbox(r, affects_docs, "affects_doc_oid",10, width_of_listbox, 0, NULL);
      affects_docs->list = db_fetch_list(db, CL_AFFECTS_DOCS, NULL);
      if (!affects_docs->list)
         return sh_message_return(r, sid->key, "Failed to fetch affects_docs list", "", A_MAIN);
      set_selection_listbox(r->pool, affects_docs, record->affects_docs.oid );
   }

   if ( marketing_revs ) 
   {
      set_listbox(r, marketing_revs, "marketing_oid", 	11, width_of_listbox, 0, NULL);
      marketing_revs->list = db_fetch_list(db, CL_MARKET_REV, record->product.oid );
      if (!marketing_revs->list)
         return sh_message_return(r, sid->key, "Failed to fetch marketing_revs list", record->product.oid , A_MAIN);
      set_selection_listbox(r->pool, marketing_revs, record->market_rev.oid );
   }

   if ( pam_releases )
   {
      set_listbox(r, pam_releases, "pam_release_oid", 11, width_of_listbox, 0, NULL);
      pam_releases->list = db_fetch_list(db, CL_PAM, record->product.oid );
      if (!pam_releases->list)
         return sh_message_return(r, sid->key, "Failed to fetch pam_release list", "", A_MAIN);
      set_selection_listbox(r->pool, pam_releases, record->pam_release.oid );
   } 

   if ( level_of_effort )
   {
      set_textbox(r, level_of_effort, "level_of_effort", 12, 0, MAX_LEVEL_OF_EFFORT, NULL);
      if(record->level_of_effort)
         level_of_effort->value = record->level_of_effort;
   }

   if ( release_date )
   {
      set_textbox(r, release_date, "release_date", 13, 0, MAX_DATE, NULL); 
      release_date->value = record->release_date;
   }

   if ( desc_request )
   {
      set_textarea(r, desc_request, "request", 14,  10, 60, MAX_REQUEST, NULL); 
      desc_request->value = record->desc_request;
   }

   if ( desc_resolution )
   {
      set_textarea(r, desc_resolution, "resolution", 15,  6, 60, MAX_RESOLUTION_DESC, NULL); 
      desc_resolution->value = record->desc_resolution;
   }

   if ( desc_verification )
   {
      set_textarea(r, desc_verification, "verification", 16,  6, 60, MAX_VERIFICATION_DESC, NULL);
      desc_verification->value = record->desc_verification;
   }

   if ( fixed_in_comp_revs )
   {
      set_listbox(r, fixed_in_comp_revs, "fixed_in_comp_rev_oid", 11, width_of_listbox, 0, NULL);
      fixed_in_comp_revs->list = db_fetch_list(db, CL_COMP_REV, record->component.oid);
      if (!fixed_in_comp_revs->list)
         return sh_message_return(r, sid->key, "Failed to fetch comp_rev list", "", A_MAIN);
      set_selection_listbox(r->pool, fixed_in_comp_revs, record->fixed_in_comp_rev.oid );
   }

   if ( comp_revs )
   {
      set_listbox(r, comp_revs, "comp_rev_oid", 11, width_of_listbox, 0, NULL);
      comp_revs->list = db_fetch_list(db, CL_COMP_REV, record->component.oid );
      if (!comp_revs->list)
         return sh_message_return(r, sid->key, "Failed to fetch comp_rev list", "", A_MAIN);
      set_selection_listbox(r->pool, comp_revs, record->comp_rev.oid );
   }

   if ( finding_activities ) 
   {
     set_listbox(r, finding_activities, "finding_act_oid", 2, width_of_listbox, 0, NULL);
     finding_activities->list = db_fetch_list(db, CL_FINDING_ACT, NULL);
     if (!finding_activities->list)
        return sh_message_return(r, sid->key, "Failed to fetch finding_activities list", "", A_MAIN);
     set_selection_listbox(r->pool, finding_activities, record->finding_activity.oid );
   }

   if (log_entry)
      set_textarea(r, log_entry, "comment", 22, 6, 60, MAX_COMMENT, NULL);


   ///// 
   sh_start(r);
   sh_js_help(r);
   sh_form_shrs(r, "Process Request", 94, 0, NULL, "right");
   if (is_checked_out) sh_button(r, 1, 1, "Save", "document.forms['shrs'].submit();");
   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();");
   sh_row_space(r);
   if(!is_checked_out)
   {
      sh_fcell_c(r, 6);
      ap_rprintf(r, "View Only: This record is currently checked out by %s.", checkedout->lockedby);
      sh_row_space(r);
   } 

   sh_next_row(r);
   sh_fcell_w(r, 1, 14);
   ap_rputs("Record No.", r);

   sh_ncell_w(r, 2, 28);
   ap_rprintf(r, "&nbsp;%s", record->oid);

   sh_ncell_w(r, 1, 14);
   ap_rputs("Submitted", r);

   sh_ncell_w(r, 2, 28);
   ap_rprintf(r, "&nbsp;%s", record->date_submitted);

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Status", "status");

   sh_ncell(r, 2);
   if (status)
      sh_listbox(r, status); 
   else
      ap_rprintf(r, "&nbsp;%s", record->status.value); 

   sh_ncell(r, 1); 
   ap_rputs("Last Modified", r); 

   sh_ncell(r, 2); 
   ap_rprintf(r, "&nbsp;%s", record->date_last_modified); 

   sh_next_row(r); 
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Priority", "priority"); 

   sh_ncell(r, 2); 
   if (priorities)
      sh_listbox(r, priorities); 
   else
      ap_rprintf(r, "&nbsp;%s", record->priority.value); 

   sh_ncell(r, 1); 
   ap_rputs("Submitter", r); 

   sh_ncell(r, 2);
   ap_rprintf(r, "&nbsp;%s", record->submitter.value);

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Severity", "severity");

   sh_ncell(r, 2);
   if (severities)
      sh_listbox(r, severities); 
   else
      ap_rprintf(r, "&nbsp;%s", record->severity.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Assigned To", "assignedto");

   sh_ncell(r, 2);
   if (assigned_to)
      sh_listbox(r, assigned_to); 
   else
      ap_rprintf(r, "&nbsp;%s", record->assigned_to.value); 


   sh_row_div(r, "");
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Feature", "feature");

   sh_ncell(r, 2);
   if (feature)
      sh_listbox(r, feature); 
   else
      ap_rprintf(r, "&nbsp;%s", record->feature.value); 

    
   sh_ncell(r, 1);
   sh_a_help(r, 1, "Platform", "platform");

   sh_ncell(r, 2);
   if (platforms)
      sh_listbox(r, platforms); 
   else
      ap_rprintf(r, "&nbsp;%s", record->platform.value); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Originator", "originator");

   sh_ncell(r, 2);
   if (originators)
      sh_listbox(r, originators); 
   else
      ap_rprintf(r, "&nbsp;%s", record->originator.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Product", "product");

   sh_ncell(r, 2);
   if (products)
      sh_listbox(r, products); 
   else
      ap_rprintf(r, "&nbsp;%s", record->product.value); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Reproducibility", "reproducibility");

   sh_ncell(r, 2);
   if (reproducibility)
      sh_listbox(r, reproducibility); 
   else
      ap_rprintf(r, "&nbsp;%s", record->reproducibility.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Marketing Revision", "marketingrev");

   sh_ncell(r, 2);
   if (marketing_revs)
      sh_listbox(r, marketing_revs); 
   else
      ap_rprintf(r, "&nbsp;%s", record->market_rev.value); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Type of Request", "typeofrequest");

   sh_ncell(r, 2);
   if (request_types)
      sh_listbox(r, request_types); 
   else
      ap_rprintf(r, "&nbsp;%s", record->request_type.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "PAM Release", "pamrelease");

   ap_rputs("<TD colspan=2 >", r);
   if (pam_releases)
      sh_listbox(r, pam_releases); 
   else
      ap_rprintf(r, "&nbsp;%s", record->pam_release.value); 

   sh_row_div(r, "");

   sh_fcell(r, 1); 
   sh_a_help(r, 1, "Finding Activity", "findingactivity");

   sh_ncell(r, 5);
   if (finding_activities)
      sh_listbox(r, finding_activities); 
   else
      ap_rprintf(r, "&nbsp;%s", record->finding_activity.value); 

   sh_next_row(r);
   sh_fcell(r, 1); 
   sh_a_help(r, 1, "Finding Mode", "findingmode");

   sh_ncell(r, 2);
   if (finding_modes)
      sh_listbox(r, finding_modes); 
   else
      ap_rprintf(r, "&nbsp;%s", record->finding_mode.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component", "component");
   sh_ncell(r, 2);  
   if (components)
      sh_listbox(r, components); 
   else
      ap_rprintf(r, "&nbsp;%s", record->component.value); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Affects<BR>Documentation", "affectsdocs");

   sh_ncell(r, 2);
   if (affects_docs)
      sh_listbox(r, affects_docs); 
   else
      ap_rprintf(r, "&nbsp;%s", record->affects_docs.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component Type", "componenttype");
   sh_ncell(r, 2);   
   ap_rprintf(r, "&nbsp;%s", record->comp_type); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Level Of Effort", "levelofeffort");

   sh_ncell(r, 2);
   if(level_of_effort)
      sh_textbox(r, level_of_effort);
   else if (record->level_of_effort)
      ap_rprintf(r, "&nbsp;%s", record->level_of_effort);

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Component Rev", "componentrev");

   sh_ncell(r, 2);
   if (comp_revs)
      sh_listbox(r, comp_revs); 
   else
      ap_rprintf(r, "&nbsp;%s", record->comp_rev.value); 

   sh_next_row(r); 
   sh_fcell(r, 3);
   ap_rputs("&nbsp;", r); 

   sh_ncell(r, 1);
   ap_rputs("Component Mgr", r);

   sh_ncell(r, 2);
   ap_rprintf(r, "&nbsp;%s", record->comp_mgr.value);

   sh_row_div(r, "");

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Title", "title");

   sh_ncell(r, 5);
   ap_rprintf(r, "%s<BR>", record->title);

   sh_row_space(r);

   sh_next_row(r);
   sh_fcell_v(r, 1);
   sh_a_help(r, 1, "Description of:</BR>Defect or <BR>New Feature,<BR>How To Reproduce,<BR>Test Environment,<BR>Implication,<BR>Suggested Fix,<BR>Attachments</a>", "desc" );

   sh_ncell_v(r, 5);
   // sh_textarea(r, 23, 10, 30000, "request", "&nbsp");
   if ( desc_request )
      sh_textarea(r, desc_request);
   else
      ap_rprintf(r, "&nbsp;%s", record->desc_request); 

   sh_row_div(r, "");
   sh_fcell_v(r, 1);
   sh_a_help(r, 1, "Comments", "comments");

   sh_ncell(r, 5);
   if(log_entry)  sh_textarea(r, log_entry);

   if(record->notes)
   {
      sh_row_space(r);
      for(i=0;i< record->notes->size;i++)
      {
         pse = record->notes->pp[i];
         sh_next_row(r);
         sh_fcell_v(r, 1);
         // ap_rprintf(r, "%s ", pse->entrydate );
         ap_rprintf(r, "%s", pse->user_name);
         sh_ncell(r, 5);
         ap_rprintf(r, "%s<BR>", pse->text);
      }
   } 

   sh_row_div(r, "");

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Resolution", "resolution");

   sh_ncell(r, 2);
   if (resolutions)
      sh_listbox(r, resolutions); 
   else
      ap_rprintf(r, "&nbsp;%s", record->resolution.value); 

   sh_ncell(r, 1);
   sh_a_help(r, 1, "Fixed In<BR>Revision No.", "fixedinrev");

   sh_ncell(r, 2);
   if (fixed_in_comp_revs)
      sh_listbox(r, fixed_in_comp_revs); 
   else
      ap_rprintf(r, "&nbsp;%s", record->fixed_in_comp_rev.value); 

   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Description of:<BR>Resolution and<BR>Test Case<BR>Suggestion", "resolutiondesc");

   sh_ncell(r, 5);
   if ( desc_resolution )
      sh_textarea(r, desc_resolution);
   else
      ap_rprintf(r, "&nbsp;%s", record->desc_resolution); 
      
   sh_next_row(r);
   sh_fcell(r, 1);
   sh_a_help(r, 1, "Release Date", "releasedate");

   sh_ncell(r, 5);
   if ( release_date )
      sh_textbox(r, release_date);
   else
      ap_rprintf(r, "&nbsp;%s", record->release_date); 
      
   sh_row_div(r, "");

   sh_fcell(r, 1);
   sh_a_help(r, 1, "Verified By", "verifiedby");

   sh_ncell(r, 5);
   if (verified_by)
      sh_listbox(r, verified_by); 
   else
      ap_rprintf(r, "&nbsp;%s", record->verified_by.value); 

   sh_next_row(r);
   sh_fcell_v(r, 1);
   sh_a_help(r, 1, "Description of:<BR>Verification Method<BR>and Test Case ID", "verificationdesc" );

   sh_ncell(r, 5);
   if (desc_verification)
      sh_textarea(r, desc_verification);
   else
      ap_rprintf(r, "&nbsp;%s", record->desc_verification); 

   sh_row_div(r, "");

   sh_ncell(r, 1);
   ap_rputs("Status History", r);
   sh_ncell(r, 5);
   if(record->status_history)
   {
      for(i=0;i< record->status_history->size;i++)
      {
         pse = record->status_history->pp[i];
         // ap_rprintf(r, "%s ", pse->entrydate );
         ap_rprintf(r, "%s ", pse->user_name);
         ap_rprintf(r, "%s<BR>", pse->text);
      }
   } 

   ap_rputs("</td></tr></table>", r); 
   sh_hidden_val(r, "formaction", A_SAVE_REQUEST);
   sh_hidden(r, &the_request_oid);
   sh_hidden(r, &starting_status);
   sh_hidden_str(r, "sid", sid->key);

   ap_rputs("</form><form name=cancel action=/shrs  method=post >", r );
   if(is_checked_out)
      sh_hidden_val(r, "formaction", A_CHECKIN_REQUEST);
   else
      sh_hidden_val(r, "formaction", A_MAIN);
   sh_hidden(r, &the_request_oid);
   sh_hidden_str(r, "sid", sid->key);
   ap_rputs("</form></body></html>", r);
   return OK;
}

void h_ph_checkin_request(ApacheRequest *req, session* sid, database *db)
{
   char* request_oid;
   request_oid = (char*) ApacheRequest_param(req, "request_oid");
   if (request_oid) db_checkin(db, request_oid, sid->user_oid );
}


/////////////////////////////////////////////////////////
//   ut->qa_mgr;
//   ut->comp_super;
//   ut->comp_mgr;
//   ut->market_mgr;
//   ut->submitter;
//   ut->assigned_to;
//   ut->verified_by;



//////////////////////////////////////////////
//oid = 
//date_submitted = 
//date_last_modified = 
//release_date = 
//title = 
//feature.oid  = 
//feature.value = 
//submitter.oid  =    
//submitter.value  =    
//assigned_to.oid  = 
//assigned_to.value  = 
//verified_by.oid  = 
//verified_by.value  = 
//platform.oid  = 
//platform.value  = 
//originator.oid  = 
//originator.value  = 
//resolution.oid  = 
//resolution.value  = 
//request_type.oid  = 
//request_type.value  = 
//finding_activity.oid  = 
//finding_activity.value  = 
//product.oid  = 
//product.value  = 
//pam_release.oid  = 
//pam_release.value  = 
//market_rev.oid  = 
//market_rev.value  = 
//component.oid  = 
//component.value  = 
//comp_rev.oid  = 
//comp_rev.value  = 
//comp_type = 
//fixed_in_comp_rev.oid = 
//fixed_in_comp_rev.value  = 
//status.oid = 
//status.value  = 
//priority.oid  = 
//priority.value  = 
//severity.oid  = 
//severity.value  = 
//reproducibility.oid  = 
//reproducibility.value  = 
//finding_mode.oid  = 
//finding_mode.value  = 
//affects_docs.oid  = 
//affects_docs.value  = 
//level_of_effort = 
//desc_request = 
//desc_resolution = 
//desc_verification = 
//comments = 
//status_history = 
//   char* request_oid;
//   char* feature_oid;
//   char* request_type_oid;
//   char* assigned_to_oid;
//   char* verified_by_oid;
//   char* status_oid;
//   char* severity_oid;
//   char* priority_oid;
//   char* reproducibility_oid;
//   char* platform_oid;
//   char* product_oid;
//   char* originator_oid;
//   char* component_oid;
//   char* comp_rev_oid;
//   char* fixed_in_comp_rev_oid;
//   char* finding_act_oid;
//   char* finding_mode_oid;
//   char* marketing_rev_oid;
//   char* level_of_effort;
//   char* pam_release_oid;
//   char* affects_doc_oid;
//   char* resolution_oid;
//   char* resolution;
//   char* release_date;
//   char* request;
//   char* verification;

//typedef struct {
//   char *current_user_oid;
//   char *comp_mgr_oid;
//   char *submitter_oid;
//   char *assigned_to_oid;
//   char *verified_by_oid;

//   char *qa_mgr_oid;
//   char *comp_super_oid;
//   char *market_mgr_oid;
//} email_canidates;
