
/*
 * jscript.c
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

// Java Scripts
#include "jscript.h"

void sh_js_help(request_rec *r)
{
   ap_rputs("\n\nfunction openHelp(strHelpTopic) {\n", r);
   ap_rputs("   var wndHelp;\n", r);
   ap_rputs("   var path = '/shrs_help.html#' + strHelpTopic;\n", r);
   ap_rputs("   wndHelp = window.open( path ,'', 'height=200, width=300, alwaysRaised=yes, scrollbars=yes');\n", r);
   ap_rputs("}\n\n", r);
}

void sh_js_cancel(request_rec *r, int formaction)
{
   ap_rputs("static void cancel() {\n", r );
   ap_rputs("   document.forms['cancel'].submit();\n", r);
   ap_rputs("}\n\n", r);
   ap_rprintf(r, "   document.forms['cancel'].formaction.value=%i;\n", formaction);  
}


void sh_js_validate_string1(request_rec *r)
{
   ap_rputs("static void validateString(strValue, strRef){\n", r);
   ap_rputs("   var i;\n var cTemp;\n", r);
   ap_rputs("   for(i=0;i<strValue.length;i++) {", r);
   ap_rputs("      cTemp = strValue.substring(i,i+1);", r);
   ap_rputs("      if ( strRef.indexOf (cTemp, 0) == -1 )", r); 
   ap_rputs("         false;\n      }\n", r);
   ap_rputs("   return true;\n}\n", r);
}


void sh_js_validate_record_number(request_rec *r)
{ 
   ap_rputs("static void validateNumericRecordNumber() {\n", r);
   ap_rputs("   var buf;\n", r);
   ap_rputs("   buf = document.RequestRecordNumber.RecordNumber.value;\n", r);
   ap_rputs("   if ( buf == '' ) {\n", r);
   ap_rputs("      alert('Please enter a record number.');\n", r);
   ap_rputs("      document.RequestRecordNumber.RecordNumber.focus() ;\n", r);
   ap_rputs("      return false;\n", r);
   ap_rputs("   }\n", r);
   ap_rputs("   if( ( buf.length > 6 ) || ( buf.length <= 0 ) )\n", r);
   ap_rputs("   {\n", r);
   ap_rputs("      alert('Record numbers should be 1 to 6 digits.');\n", r);
   ap_rputs("      document.RequestRecordNumber.RecordNumber.focus();\n", r);
   ap_rputs("      return false;\n", r);
   ap_rputs("   }\n", r);
   ap_rputs("   if ( !validateString(document.RequestRecordNumber.RecordNumber.value, '0123456789')) {\n", r);
   ap_rputs("      alert('Record numbers must be numeric.');\n", r);
   ap_rputs("      document.RequestRecordNumber.RecordNumber.focus();\n", r);
   ap_rputs("      return false;\n", r);
   ap_rputs("   }\n", r);
   ap_rputs("   return true;\n}\n\n", r);
}
