/*
 * form.h
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

#ifndef _FORM_H
#define _FORM_H

#include "httpd.h"
#include "shrs.h"
#include "twodim.h"
#include "db_shrs.h"

//#include "http_config.h"
//#include "http_protocol.h"
//#include "ap_config.h"
//#include "http_log.h"
//#include "apache_request.h"

/////////////////////////
// Function Declarations
//
// Functions that return a query string begin with 
//        qs_ (query select), qi_ (query insert), or qu_ (query_update).
//        qx_ functions are followed by an identifier like lu_ , 
//        and then an identifier for each field affected by the action.
//
// Functions that begin with 
//   ml_	return max length for a given table and field.
//
//   sh_js_	output java script for html.
//
//   sh_s_ 	output html listbox input fields derived from either global data or record result set.
//   sh_ms_ 	are multiselect
//   h_		functions called within the handler switch state
//   h_sh	are functions within the handler switch that output html pages
//   h_ph	are functions within the handler switch that process posted data.
// 

/////////////////////////////////////////////////// 
// typedef struct {
//    char *s;
//    int size;
// } _nchar;
// 
// typedef struct {
//    char *oid;
//    _nchar str;
// } _str_rec;
//
// typedef struct {
//    const char * const label;
//    void (*pfun)(request_rec*, int, int[]);
// } state;


typedef struct {
   char* name;
   char* value;
} hidden;

typedef struct {
   char* name;
   int tabindex;
   int width;
   int multiple;
   char* onchange;
   twodim *list;   
} listbox;

typedef struct {
   char* name;
   int tabindex;
   char* label;
   int   ischecked;
} checkbox;

typedef struct {
   char* name;
   int tabindex;
   int size;
   int maxlength;
   char* onchange;
   char* value;  
} textbox, password;

typedef struct {
   char* name;
   int tabindex;
   int rows;
   int cols;
   int maxlength;
   char* onchange;
   char* value;
} textarea;

typedef struct {
  hidden cancel; 
} cancel_form;


//typedef struct {
//   int account;
//   int password;
//   int last_name;
//   int first_name;
//   int initials;
//   int email;
//} form_user_maxchar;

//////////////////
// form.c
void sh_nbsp(request_rec *r, int number);

void sh_start(request_rec *r );

void sh_end_shrs(request_rec *r, char* key, int rm_action, int cancel_action);
void sh_form_shrs(request_rec *r, 
	const char *title, int width, int border, const char *control_focus, const char *button_alignment);
void sh_button(request_rec *r, int tabindex, int nbspaces, const char *name_value, const char *onclick);
void sh_formaction_submit_button(request_rec *r, 
	int tabindex, int nbspaces, const char* label, int formaction );
void sh_param_submit_button(request_rec *r, int tabindex, char *name, char *value);
void sh_row_div(request_rec *r, const char* label);
void sh_row_space(request_rec *r);
void sh_next_row(request_rec *r);
void sh_fcell(request_rec *r, int colspan);
void sh_fcell_c(request_rec *r, int colspan);
void sh_fcell_vc(request_rec *r, int colspan);
void sh_fcell_v(request_rec *r, int colspan);
void sh_fcell_w(request_rec *r, int colspan, int width);
void sh_ncell(request_rec *r, int colspan);
void sh_ncell_c(request_rec *r, int colspan);
void sh_ncell_vc(request_rec *r, int colspan);
void sh_ncell_r(request_rec *r, int colspan);
void sh_ncell_v(request_rec *r, int colspan);
void sh_ncell_w(request_rec *r, int colspan, int width);
void sh_a_help(request_rec *r, int tabindex, const char *label, const char *help_file_name);

void set_hidden(hidden* h, char* name, char* value);
void sh_hidden(request_rec *r, hidden* h);
void sh_hidden_str(request_rec *r, const char* name, char* str);
void sh_hidden_val(request_rec *r, const char* name, int val); 

void sh_s_numbers(request_rec *r, int tabindex, const char* sh_s_name, int limit);
void sh_s_year(request_rec *r, int tabindex, const char *sh_s_name);

checkbox* create_checkbox(request_rec *r);
void set_checkbox(request_rec *r, checkbox *c, char* name, int tabindex, char* label);
void check_checkbox(checkbox *c); 
void uncheck_checkbox(checkbox *c);
void sh_checkbox(request_rec *r, checkbox *);

textbox* create_textbox(request_rec *r);
void set_textbox(request_rec *r, textbox *t, char* name, int tabindex, int size, int maxlength, char* onchange);
void sh_textbox(request_rec *r, textbox*); 
void sh_textbox_pwd(request_rec *r, textbox* );

void option_unspecified(request_rec *r, int width);

listbox* create_listbox(request_rec *r);
void set_listbox(request_rec *r, listbox*s, char* name, int tabindex, int width, int multiple, char* onchange);
void set_selection_listbox(ap_pool* pool, listbox *s, char* oid);
twodim* get_list(listbox *l);
void sh_listbox(request_rec *r, listbox* l);

textarea* create_textarea(request_rec *r);
void set_textarea(request_rec *r, textarea* t, const char* name, int tabindex, 
				int rows, int cols, int maxlength, char* onchange);
void sh_textarea(request_rec *r, textarea *); 

int sh_twodim(request_rec *r, twodim* td);
int h_sh_twodim_test(request_rec *r);

int sh_outoforder(request_rec *r, char* );
int sh_message_login(request_rec *r, char* msg);
int sh_message_return(request_rec *r, char* key, char* caption, char* error_msg, int formaction);
int sh_message_return_hidden(request_rec *r, char* key, char* caption, char* msg, int formaction, hidden* h);
void sh_report(request_rec *r, const char *title, int width, int border, int colspan);

int h_sh_twodim_severity_test(request_rec *r, database* db);

#endif // _FORM_H
////////////////////////////////////////////////////////////
