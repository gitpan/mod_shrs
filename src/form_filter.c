/*
 * form_filter.h
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

#include "form.h"
#include "db.h"

static int h_sh_filter(request_rec *r, char* sid, database db)
{
   checkbox 	wordsearch;
   textbox	searchtext;
   checkbox	title; 
   checkbox	desc_request; 
   checkbox	desc_verification; 
   checkbox	desc_resolution; 
   checkbox	comments; 
   
   checkbox	comma;
   textbox	record_nos;
   checkbox	range;
   textbox	rec_no_from;
   textbox	rec_no_to;
   checkbox	date_submitted;

   listbox	status;
   listbox	severities;
   listbox	affects_docs;
   listbox	reproducibility;
   listbox	features;
 
   listbox	componets;
   listbox	comp_types;
   listbox	products;
   listbox	platforms;
   listbox	finding_modes;

   listbox	originators;
   listbox	resolutions;
   listbox	submitter;
   listbox	assigned_to;
   listbox	verified_by;

   // request_rec, checkbox, name, tabindex, label  
   set_checkbox(r, &wordsearch, "word_search", 0, "Word Search");

   // request_rec, textbox, name, tabindex, maxlength, size
   set_textbox(r, &searchtext, "search_text", 0, 50, 0);

   set_checkbox(r, &title, "", "Title", NULL);
   set_checkbox(r, &desc_request, "", "Description Of Request", NULL);
   set_checkbox(r, &desc_verification ,"", "Description Of Verification", NULL);
   set_checkbox(r, &desc_resolution, "", "Description Of Resolution", NULL);
   set_checkbox(r, &comments, "", "Comments", NULL);

   set_checkbox(r, &comma, "comma_delimited", 0, "Comma Delimited List of Request Numbers");
   set_textbox(r, &record_nos , "record_nos", 0, 68, 0);
   set_checkbox(r, &range, "range", 0, "Range of Request Numbers");
   set_textbox(r, &rec_no_from, "record_nos_from", 0, 0, 0);
   set_textbox(r, &rec_no_to, "record_nos_to", 0, 0, 0);
   set_checkbox(r, &date_submitted, "date_submitted", 0, "Date Submitted");

   set_listbox(r, &status, "status", 0, 0, 1);
   set_listbox(r, &severities, "severities", 0, 0, 1);
   set_listbox(r, &affects_docs, "affects_docs", 0, 0, 1);
   set_listbox(r, &reproducibility, "reproducibility", 0, 0, 1);
   set_listbox(r, &features, "features", 0, 0, 1);

   // request_rec, listbox, name, tabindex, width, multiple
   set_listbox(r, &componets, "components", 0, 0, 1);
   set_listbox(r, &comp_types, "comp_types", 0, 0, 1);
   set_listbox(r, &products, "products", 0, 0, 1);
   set_listbox(r, &platforms, "platforms", 0, 0, 1);
   set_listbox(r, &finding_modes, "finding_modes", 0, 0, 1);

   set_listbox(r, &originators, "originators",  0, 0, 0);
   set_listbox(r, &resolutions, "resolutions",  0, 0, 0);
   set_listbox(r, &submitter, "submitters",  0, 0, 0);
   set_listbox(r, &assigned_to, "assigned_tos",  0, 0, 0);
   set_listbox(r, &verified_by, "verified_bys",  0, 0, 0);

   ////////////////////////// 
   sh_start(r);

   sh_form(r, "Filter", 100, 0, NULL, "right");
   sh_button(r, 4, 1, "Report", "");
   sh_button(r, 3, 1, "Summary", "");
   sh_button(r, 1, 1, "Cancel", "document.forms['cancel'].submit();");
   sh_row_space(r);

   sh_ncell(r, 2);
   ap_rputs("Stored Querys&nbsp;&nbsp;", r);
   ap_rputs("<select name=stored_query >", r);
   ap_rputs("<OPTION value=0 >Empty</OPTION>", r);
   ap_rputs("</select>", r);

   sh_ncell(r, 4);
   sh_button(r, 6, 1, "Save Query", "" );
   sh_button(r, 7, 1, "Delete Query", "" );

   sh_row_div(r, "");

   sh_ncell(r, 1);
   sh_checkbox(r, 0, "", "Word Search", NULL );
   sh_ncell(r, 5);
   sh_textbox(r, 0, 68, "search", 60, NULL);

   sh_next_row(r);
   sh_fcell(r, 1);
   ap_rputs("&nbsp;", r);
   sh_ncell(r, 5);

   sh_ncell(r, 2);
   sh_checkbox(r, 0, "", "Title", NULL);
   sh_checkbox(r, 0, "", "Description Of Request", NULL);
   sh_checkbox(r, 0, "", "Description Of Verification", NULL);
   sh_checkbox(r, 0, "", "Description Of Resolution", NULL);
   sh_checkbox(r, 0, "", "Comments", NULL);


   sh_row_space(r);

   sh_ncell(r, 2);
   sh_checkbox(r, &comma); 

   sh_ncell(r, 4);
   sh_textbox(r, &record_no);

   sh_ncell(r, 2);

   sh_next_row(r);
   sh_fcell(r, 2);
   sh_checkbox(r, &range);

   sh_ncell(r, 2);

   sh_ncell(r, 2);
   ap_rputs("&nbsp;&nbsp;FROM&nbsp;&nbsp;", r);
   sh_textbox(r, &rec_nos_from");

   sh_ncell(r, 2);
   ap_rputs("&nbsp;&nbsp;TO&nbsp;&nbsp;", r);

   sh_textbox(r, "rec_nos_to");

   sh_row_space(r);

   ap_rputs("<!-- DATES -->", r);

   //////////////////
   // Submission Dates 
   sh_ncell(r, 1);
   sh_checkbox(r, &date_submitted); 

   // Submit Before
   sh_ncell(r, 2);
   ap_rputs("Before&nbsp;&nbsp;", r);
   sh_s_numbers(r, 0, "submit_bm", 12);
   sh_s_numbers(r, 0, "submit_bd", 31);
   sh_s_year(r, 0, "submit_by");

   // Submit After
   sh_ncell(r, 3);
   ap_rputs("After&nbsp;&nbsp;", r);
   sh_s_numbers(r, 0, "submit_am", 12);
   sh_s_numbers(r, 0, "submit_ad", 31);
   sh_s_year(r, 0, "submit_ay");

   sh_row_space(r);

   sh_ncell_c(r, 1);
   ap_rputs("Status", r);

   sh_ncell_c(r, 1);
   ap_rputs("Severities", r);

   sh_ncell_c(r, 1);
   ap_rputs("Priorities", r);

   sh_ncell_c(r, 1);
   ap_rputs("Affects Docs", r);

   sh_ncell_c(r, 1);
   ap_rputs("Reproducibility", r);

   sh_ncell_c(r, 1);
   ap_rputs("Feature", r);

   sh_next_row(r);
   sh_fcell_vc(r, 1);
   sh_listbox(r, &status);
    
   sh_ncell_vc(r, 1);
   sh_listbox(r, &severities); 

   sh_ncell_vc(r, 1);
   sh_listbox(r, &priorities);

   sh_ncell_vc(r, 1);
   sh_listbox(r, &affects_docs);

   sh_ncell_vc(r, 1);
   sh_listbox(r, &reproducibility);

   sh_ncell_vc(r, 1);
   sh_listbox(r, &features);

   sh_row_space(r);

   sh_ncell_c(r, 1);
   ap_rputs("Components", r);

   sh_ncell_c(r, 1);
   ap_rputs("Comp Types", r);

   sh_ncell_c(r, 1);
   ap_rputs("Products", r);

   sh_ncell_c(r, 1);
   ap_rputs("Platforms", r);

   sh_ncell_c(r, 1);
   ap_rputs("Finding Activities", r);

   sh_ncell_c(r, 1);
   ap_rputs("Finding Modes", r);

   sh_next_row(r);
   sh_fcell_vc(r, 1);
   sh_listbox(r, &component_oid);

   sh_ncell_vc(r, 1);
   sh_listbox(r, &comp_type_oid);

   sh_ncell_vc(r, 1);
   sh_listbox(r, &product_oid);

   sh_ncell_vc(r, 1);
   sh_listbox(r, &platform_oid);

   sh_ncell_vc(r, 1);
   sh_listbox(r, finding_modes );

   sh_ncell_vc(r, 1);
   sh_listbox(r, finding_modes );

   sh_next_row(r);
   sh_fcell_c(r, 1);
   ap_rputs("Originators", r);

   sh_ncell_c(r, 1);
   ap_rputs("Resoltions", r);

   sh_ncell_c(r, 1);
   ap_rputs("Submitter", r);

   sh_ncell_c(r, 1);
   ap_rputs("Assigned To", r);

   sh_ncell_c(r, 1);
   ap_rputs("Verified By", r);

   sh_ncell_c(r, 1);
   ap_rputs("&nbsp;", r);

   sh_row_space(r);

   sh_ncell_vc(r, 1);
   sh_listbox(r,  &originators);

   sh_ncell_c(r, 1);
   sh_listbox(r,  &resolutions);

   sh_ncell_c(r, 1);
   sh_listbox(r,  &submitters);

   sh_ncell_c(r, 1);
   sh_listbox(r,  &assigned_tos);

   sh_ncell_c(r, 1);
   sh_listbox(r,  &verified_bys);

   sh_ncell_c(r, 2);
   ap_rputs("&nbsp;", r);

   sh_end(r, sid, 0, A_MAIN );
   return OK;
}

