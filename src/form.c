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

#include "form.h"
#include "apache_request.h"
#include "util.h"

void sh_nbsp(request_rec *r, int number)
{
   int i; if (number>0)  for(i=0; i<number; i++) ap_rputs("&nbsp;", r);
}

void sh_start(request_rec *r )
{
   r->content_type = "text/html";
   ap_send_http_header(r);
   ap_rputs("<html><head><title>www.johnatkins.net</title>", r);
   // ap_rputs("<meta http-equiv=pragma content=\"no-cache\">", r);
   // ap_rputs("<meta http-equiv=\"expires\" content=\"0\">", r);
   // ap_rputs("<!-- <link rel=stylesheet href=\"shrs.css\" type=\"text/css\" > -->", r);
   ap_rputs("<script language=\"JavaScript\" > <!--// ", r);
}

void sh_end_shrs(request_rec *r, char* key, int next_action, int cancel_action)
{
   ap_rputs("</td></tr></table>", r); 
   sh_hidden_val(r, "formaction", next_action);
   if (key) sh_hidden_str(r, "sid", key);
   ap_rputs("</form><form name=cancel action=\"shrs\"  method=post >", r );
   sh_hidden_val(r, "formaction", cancel_action);
   if (key) sh_hidden_str(r, "sid", key);
   ap_rputs("</form></body></html>", r);
}

void sh_form_shrs(request_rec *r, const char *title, int width, int border, 
				const char *control_focus, const char *button_alignment)
{
   ap_rputs(" //--> </script></head><body", r);

   if (control_focus) 
     ap_rprintf(r, " onLoad=\"document.shrs.%s.focus();\" ", control_focus);
 
   ap_rputs("><form name=\"shrs\" action=\"/shrs\" method=\"post\" >", r);

   ap_rprintf(r, "<table width=\"%i%\" border=%i cellpadding=0 cellspacing=0 align=left>", width , border);

   ap_rputs("<tr ><td colspan=\"6\" >&nbsp;</td></tr>", r);
   ap_rprintf(r, "<tr ><td colspan=\"6\" align=center ><strong>%s</strong></td></tr>", title );
   ap_rputs("<tr ><td colspan=\"6\" >&nbsp;</td></tr>", r);

   // Menu Bar
   ap_rprintf(r, "<tr bgcolor=\"#d3d3d3\" ><td colspan=6 align=%s >", button_alignment);

   // menu buttons follow this function
}

void sh_end_bs(request_rec *r, char* key, int next_action, int cancel_action)
{
   ap_rputs("</td></tr></table>", r); 
   ap_rputs("</form><form name=cancel action=/bs  method=post >", r );
   ap_rputs("</form></body></html>\r\n", r);
}

void sh_form_bs(request_rec *r, const char *title, int width, int border, 
				const char *control_focus, const char *button_alignment)
{
   ap_rputs("//--></script></head><body", r);

   if (control_focus) 
      ap_rprintf(r, " onLoad=\"document.bs.%s.focus();\" ", control_focus);
 
   ap_rputs("><form name=bs action=/bs method=post >", r);

   ap_rprintf(r, "<table width=\"%i%\" border=%i cellpadding=0 cellspacing=0 align=left>", width , border);

   ap_rputs("<tr ><td colspan=\"6\" >&nbsp;</td></tr>", r);
   ap_rprintf(r, "<tr ><td colspan=\"6\" align=center ><strong>%s</strong></td></tr>", title );
   ap_rputs("<tr ><td colspan=\"6\" >&nbsp;</td></tr>", r);

   // Menu Bar
   ap_rprintf(r, "<tr bgcolor=\"#d3d3d3\" ><td colspan=6 align=%s >", button_alignment);

   // buttons follow this function
}

void sh_report_shrs(request_rec *r, const char *title, int width, int border, int colspan )
{
   ap_rputs("//--></script></head><body", r); 
   ap_rputs("><form name=shrs action=/shrs method=post >", r);

   ap_rprintf(r, "<table width=\"%i%\" border=%i cellpadding=0 cellspacing=0 align=left cols=%i >", 
									width , border, colspan);
   ap_rprintf(r, "<tr ><td colspan=%i >&nbsp;</td></tr>", colspan);
   ap_rprintf(r, "<tr ><td colspan=%i  align=center ><strong>%s</strong></td></tr>", colspan, title );
   ap_rprintf(r, "<tr ><td colspan=%i >&nbsp;</td></tr>", colspan);
}


///// BUTTONS
void sh_button(request_rec *r, int tabindex, int nbspaces, const char *name_value, const char *onclick)
{
   ap_rputs("&nbsp;<input type=button value=\"&nbsp;", r);
   if (name_value) ap_rprintf(r, "%s", name_value);
   sh_nbsp(r, nbspaces);
   ap_rprintf(r, "\" onclick=%s >&nbsp;</input>", onclick); 
}

void sh_formaction_submit_button(request_rec *r, int tabindex, int nbspaces, const char* label, int formaction )
{
   ap_rprintf(r, "&nbsp;<input type=submit tabindex=%i value='&nbsp;%s", tabindex, label);
   sh_nbsp(r, nbspaces);
   ap_rprintf(r, "' onclick=\"document.forms['shrs'].formaction.value=%i; document.forms['shrs'].submit(); \" >&nbsp;</input>", formaction); 
}

void sh_param_submit_button(request_rec *r, int tabindex, char *name, char *value)
{
   ap_rprintf(r, "<input type=submit tabindex=%i name='%s' value='%s' ></input>", tabindex, name, value);
}

/////  TABLE ROW FUNCTIONS
void sh_row_div(request_rec *r, const char* label)
{
//    ap_rputs("</td></tr><tr><td colspan=6 >&nbsp;</td></tr><tr>", r);
//    ap_rprintf(r, "</td></tr><tr bgcolor=\"#d3d3d3\" ><td colspan=6 >&nbsp;%s</td></tr><tr>", label);
//    ap_rputs("</td></tr><tr><td colspan=6 >&nbsp;</td></tr><tr>", r);

   ap_rputs("</td></tr>", r);
   ap_rputs("<tr><td colspan=6 >&nbsp;</td></tr>", r);
   ap_rputs("<tr></td></tr><tr bgcolor=\"#d3d3d3\" ><td colspan=6 >&nbsp;", r);
   if (label) ap_rprintf(r,"%s", label);
   ap_rputs("</td></tr><tr><td colspan=6 >&nbsp;</td></tr>", r);
   ap_rputs("<tr>", r);
}

void sh_row_space(request_rec *r)
{
   ap_rputs("</td></tr><tr><td colspan=6 >&nbsp;</td></tr><tr>", r);
}

void sh_next_row(request_rec *r)
{
   ap_rputs("</td></tr><tr>", r);
}

/////////////// TABLE CELL FUNCTIONS 

void sh_fcell(request_rec *r, int colspan)
{
   ap_rprintf(r, "<td colspan=%i >", colspan);
}

void sh_fcell_c(request_rec *r, int colspan)
{
   ap_rprintf(r, "<td colspan=%i align=center>", colspan);
}

void sh_fcell_vc(request_rec *r, int colspan)
{
   ap_rprintf(r, "<td colspan=%i align=center valign=top>", colspan);
}

void sh_fcell_v(request_rec *r, int colspan)
{
   ap_rprintf(r, "<td colspan=%i valign=top >", colspan);
}

void sh_fcell_w(request_rec *r, int colspan, int width)
{
   ap_rprintf(r, "<td colspan=%i width=\"%i%\" >", colspan, width);
}



// Next cell in row:
void sh_ncell(request_rec *r, int colspan)
{
   ap_rprintf(r, "</td><td colspan=\"%i\" >", colspan);
}

void sh_ncell_c(request_rec *r, int colspan)
{
   ap_rprintf(r, "</td><td colspan=%i align=center>", colspan);
}

void sh_ncell_vc(request_rec *r, int colspan)
{
   ap_rprintf(r, "</td><td colspan=%i align=center valign=top>", colspan);
}

void sh_ncell_r(request_rec *r, int colspan)
{
   ap_rprintf(r, "</td><td colspan=%i align=right >", colspan);
}

void sh_ncell_v(request_rec *r, int colspan)
{
   ap_rprintf(r, "</td><td colspan=%i valign=top >", colspan);
}

void sh_ncell_w(request_rec *r, int colspan, int width)
{
   ap_rprintf(r, "</td><td colspan=%i width=\"%i%\" >", colspan, width);
}

void sh_a_help(	request_rec *r, 
		int tabindex, 
		const char *label, 
		const char *help_file_name)
{
   ap_rprintf(r, "<a tabindex=%i href=\"JavaScript:openHelp('%s')\" >%s</a>", tabindex, help_file_name, label);
}

void set_hidden(hidden* h, char* name, char* value)
{
    h->name = name;
    h->value = value;
}

void sh_hidden(request_rec *r, hidden *h)
{
   if(h)
      ap_rprintf(r, "<input type=hidden name=%s value='%s' ></input> ", h->name, h->value ); 
}

void sh_hidden_str(request_rec *r, const char* name, char* str)
{
   ap_rprintf(r, "<input type=hidden name=%s value=%s ></input> ", name, str); 
}

void sh_hidden_val(request_rec *r, const char* name, int val) 
{
   ap_rprintf(r, "<input type=hidden name=%s value=%i ></input> ", name, val); 
}

void sh_s_numbers(request_rec *r, int tabindex, const char* sh_s_name, int limit)
{
   int i;
   ap_rprintf(r, "<select name=%s tabindex=%i >", sh_s_name, tabindex );
   ap_rputs("<option selected >1</option>", r);
   for(i=2;i<=limit;i++) 
      ap_rprintf(r, "<option value=%i >%i</option>", i);
   ap_rputs("</select>", r);
}

void sh_s_year(request_rec *r, int tabindex, const char *sh_s_name)
{
   ap_rprintf(r, "<select name=%s tabindex=%i >", sh_s_name, tabindex);
   ap_rputs("<option value=2002  >2002</option>", r);
   ap_rputs("<option value=2003  >2003</option>", r);
   ap_rputs("<option value=2004  >2004</option>", r);
   ap_rputs("<option value=2005  >2005</option>", r);
   ap_rputs("<option value=2006  >2006</option>", r);
   ap_rputs("</select>", r);
}

void sh_textarea(request_rec *r, textarea *t)
{ 
   if(t) {
      ap_rprintf(r, "<textarea name=%s tabindex=%i rows=%i cols=%i  maxlength=%i ", 
   		t->name, t->tabindex, t->rows, t->cols, t->maxlength );
      if (t->onchange) ap_rprintf(r, " onchange=%s ", t->onchange);
      ap_rputs(">", r);
      if (t->value) ap_rprintf(r, "%s", t->value );
      ap_rputs("</textarea>", r);
   }
}

textarea* create_textarea(request_rec *r)
{
   textarea* ptr;
   ptr = (textarea*) ap_pcalloc(r->pool, sizeof(textarea) );
   if(ptr)
   {
      ptr->name = NULL;
      ptr->tabindex = 0;
      ptr->rows = 1;
      ptr->cols = 80; 
      ptr->onchange  = NULL;
      ptr->value = NULL;
   }
   return ptr;
}

void set_textarea(request_rec *r, textarea* t, const char* name, int tabindex, 
				int rows, int cols, int maxlength, char* onchange )
{
   t->name = ap_pstrdup(r->pool, name);
   t->tabindex = tabindex;
   t->rows = rows;
   t->cols = cols; 
   t->maxlength = maxlength;
   t->onchange  = onchange;
   t->value=NULL;
}

void option_unspecified(request_rec *r, int width)
{
   ap_rputs("<option>Unspecified", r);
   sh_nbsp(r, width - 11);
   ap_rputs("</option>", r); 
}


int sh_message_return(request_rec *r, char* key, char* caption, char* msg, int formaction) 
{
   sh_start(r);
   sh_form_shrs(r, caption, 84, 0, NULL, "right");
   sh_button(r, 1, 1, "OK", "document.forms['shrs'].submit();");
   sh_row_space(r);
   sh_fcell(r, 6);
   ap_rprintf(r, "%s", msg);
   ap_rputs("</td></tr></table>", r); 
   sh_hidden_val(r, "formaction", formaction);
   if (key) sh_hidden_str(r, "sid", key);
   ap_rputs("</form></body></html>", r);
   return OK;
}

int sh_message_return_hidden(request_rec *r, char* key, char* caption, char* msg, int formaction, hidden* h) 
{
   sh_start(r);
   sh_form_shrs(r, caption, 84, 0, NULL, "right");
   sh_button(r, 1, 1, "OK", "document.forms['shrs'].submit();");
   sh_row_space(r);
   sh_fcell(r, 6);
   ap_rprintf(r, "%s", msg);
   ap_rputs("</td></tr></table>", r); 
   sh_hidden_val(r, "formaction", formaction);
   if (h) sh_hidden(r, h);
   if (key) sh_hidden_str(r, "sid", key);
   ap_rputs("</form></body></html>", r);
   return OK;
}


int sh_message_login(request_rec *r, char* msg)
{
   sh_start(r);
   sh_form_shrs(r, msg, 84, 0, NULL, "center");
   sh_row_space(r);
   sh_fcell_c(r, 6);
   ap_rputs("<a href=\"http://www.johnatkins.net/login.html\">Login</a>", r); 
   ap_rputs("</td></tr></table></form></body></html>", r); 
   return OK;
}

int sh_outoforder(request_rec *r, char* logmsg)
{
   sh_start(r);
   sh_form_shrs(r, "Out Of Order", 84, 0, NULL, "center");
   sh_row_space(r);
   ap_rputs("</td></tr></table></form></body></html>", r); 
   return OK;
}

int sh_twodim(request_rec *r, twodim *t)
{
  int row, row_cnt, col, col_cnt;

  row_cnt = td_row_count(t); 
  col_cnt = td_get_col_count(t);

  sh_start(r);
  sh_report_shrs(r, "Printing of twodim", 100, 1, col_cnt);

  for(row=0; row<row_cnt; row++)
  {
     sh_next_row(r);    
     for(col=0; col < col_cnt; col++)
     {
        if (col == 0) {
           sh_fcell_w(r, 1, 100/col_cnt);
        } else {
           sh_ncell_w(r, 1, 100/col_cnt);
        }
        ap_rprintf(r, "(%i, %i)", row, col);
        ap_rprintf(r, "%s", td_get_val(t, row, col));
     }   
  }
  sh_next_row(r);    
  sh_fcell(r, 1);
  ap_rprintf(r, "%i", td_row_count(t));
  ap_rputs("</td></tr></table></form></body></html>", r);
  return OK;
}



////////////////////////////////////////////////////////////////////////////////
// HTML Form Constrols

// CHECKBOX
checkbox* create_checkbox(request_rec *r)
{  
   checkbox* ptr; 
   ptr = (checkbox*) ap_pcalloc(r->pool, sizeof(checkbox) );
   if (ptr)
   {
      ptr->name	= NULL; 
      ptr->label = NULL; 
      ptr->tabindex  = 0;
      ptr->ischecked = 0; 
   }
   return ptr;
}

void set_checkbox(request_rec *r, checkbox *c, char* name, int tabindex, char* label)
{  
   c->name	= ap_pstrdup(r->pool, name);
   c->label	= ap_pstrdup(r->pool, label);
   c->tabindex  = tabindex;
   c->ischecked = 1; 
}

void check_checkbox(checkbox *c)   { c->ischecked = 1; }
void uncheck_checkbox(checkbox *c) { c->ischecked = 0; }

void sh_checkbox(request_rec *r, checkbox *c) {
   ap_rprintf(r, "<input type=checkbox name='%s' tabindex='%i' ", c->name, c->tabindex); 
   if ( c->ischecked ) ap_rputs(" checked ", r); 
   ap_rprintf(r, " >&nbsp;&nbsp; %s</input>", c->label);
}

// TEXTBOX
textbox* create_textbox(request_rec *r)
{
   textbox* ptr;
   ptr = (textbox*) ap_pcalloc(r->pool, sizeof(textbox) );
   if(ptr)
   {
      ptr->name	= NULL; 
      ptr->tabindex  = 0;
      ptr->size      = 0;
      ptr->onchange  = NULL;
      ptr->value     = NULL; 
   }
   return ptr;
}

void set_textbox(request_rec *r, textbox *t, char* name, int tabindex, int size, int maxlength, char* onchange )
{
   t->name	= ap_pstrdup(r->pool, name);
   t->tabindex  = tabindex;
   t->size      = size;
   t->onchange  = onchange;
   t->maxlength = maxlength;
   t->value     = NULL; 
}

void sh_textbox(request_rec *r, textbox *t)
{
   ap_rprintf(r, "<input type=text tabindex=%i maxlength=%i ", t->tabindex, t->maxlength);
   if (t->name != NULL ) ap_rprintf(r, " name='%s' ", t->name);
   if ( t->value ) ap_rprintf(r, " value='%s' ", t->value);
   if (t->size > 0) ap_rprintf(r, " size=%i ", t->size);
   if (t->onchange) ap_rprintf(r, " onchange=%s ", t->onchange);
   ap_rputs("></input>", r);
}

void sh_password(request_rec *r, password *p)
{
   ap_rprintf(r, "<input type=password name=%s", p->name );
   ap_rprintf(r, " value='%s' ", p->value );
   ap_rputs("></input>", r);
}

// list box or select type input
listbox* create_listbox(request_rec *r)
{
   listbox *ptr;
   ptr = (listbox*) ap_palloc(r->pool, sizeof(listbox) );
   if (ptr)
   {
      ptr->name	= NULL; 
      ptr->tabindex  = 0;
      ptr->width     = 0;
      ptr->multiple  = 0;
      ptr->onchange  = NULL;
      ptr->list = NULL;  
   }
   return ptr;
}

twodim* get_list(listbox *l)
{
   return l->list;
}

void set_listbox(request_rec *r, listbox *s, char* name, int tabindex, int width, int multiple, char* onchange)
{
   s->name	= ap_pstrdup(r->pool, name);
   s->tabindex  = tabindex;
   s->width     = width;
   s->multiple  = multiple;
   s->onchange  = onchange;
   s->list = NULL;  // td_alloc_twodim(r->pool);
}

void set_selection_listbox(ap_pool* pool, listbox *s, char* oid)
{
   int i;
   if (s && s->list) {
      for(i=0;i<td_row_count(s->list);i++)
          if (strcmp(td_get_val(s->list, i, 0), oid) == 0) td_set_val(pool, s->list, i, 2, "y" );
   }
}

void sh_listbox(request_rec *r, listbox *s)
{
   twodim* td;
   char* cptr;
   int i, len;
   if (s) {
     ap_rprintf(r, "<select name='%s' tabindex=%i ", s->name, s->tabindex);
     if ( s->multiple == 1 ) ap_rputs(" multiple ", r);
     if ( s->onchange ) ap_rprintf(r, " onchange=\"%s\"", s->onchange);
     ap_rputs(" >", r);
     if (s->list) {
        for(i=0; i < td_row_count( s->list) ; i++)
        {
           ap_rprintf(r, "<option value=%s ", td_get_val( s->list, i, 0) );
     
           if ( strcmp(td_get_val( s->list, i, 2  ), "y") == 0 ) ap_rputs(" selected ", r);
     
           if ( atoi(td_get_val( s->list, i, 0))  == 1 ) {
              cptr = td_get_val(s->list, i, 1);
              len = strlen(cptr);
              ap_rprintf(r, " >%s", cptr );
              sh_nbsp(r, s->width - len );
              ap_rputs("</option>", r); 
           } else {
              ap_rprintf( r, " >%s</option>", td_get_val( s->list, i, 1) ); 
           }
        }
     }
     ap_rputs("</select>", r);
   }
}
//////////////////////////////////////////////
