/*
 * util.h
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

#include "util.h"

int spacecount(const char *str)
{
   int count = 0;
   char* ptr;

   if (!*str) return 0;

   if (strlen(str) == 0) return 0; 

   ptr = (char*)str;
   while( *ptr ) {
      if ( *ptr == 32 ) count++;
      ++ptr;
   }
   return count;
}

char* crlf_to_br(ap_pool* pool, const char *str)
{
   int q_count = 0;
   char* ptr;
   char* rtn_str = NULL;

   if ( !*str ) return NULL;
   if ( strlen(str) == 0 ) return "";

   ptr = (char*)str; 
   while ( *str ) 
   {
      if ( *str == 13 ) 
      {
          rtn_str = ap_pstrcat(pool, ap_pstrndup(pool, ptr, str - ptr), "<BR>", rtn_str, "\\", NULL);
          str++;
          if ( *str == 10 ) str++;
          ptr  = (char *)str;
      }
   }
   rtn_str = ap_pstrcat(pool, ap_pstrndup(pool,  ptr, str - ptr), NULL);
   return rtn_str;
}


char* escape_single_quotes(ap_pool *pool, const char *str)
{
   int q_count = 0;
   char* ptr;
   char* rtn_str = NULL;

   if ( !str ) return NULL;
   if ( strlen(str) == 0 ) return (char*)str;

   ptr = (char*)str; 
   while ( *str ) 
   {
      if ( *str == 39 ) 
      {
          rtn_str = ap_pstrcat(pool, ap_pstrndup(pool, ptr, str - ptr), rtn_str, "\\", NULL);
          ptr  = (char *)str;
      } 
      ++str;
   }
   rtn_str = ap_pstrcat(pool, ap_pstrndup(pool,  ptr, str - ptr), NULL);
   return rtn_str;
}


// char* escape_single_quotes(ap_pool *pool, const char *str)
// {
//    int q_count = 0;
//    char* ptr;
//    char* rtn_str = NULL;
// 
//    if ( str == NULL ) return NULL;
//    if ( strlen(str) == 0 ) return "";
// 
//    // count the number of single quotes to escape
//    ptr = (char*)str;
//    while( *ptr ) {
//       if ( *ptr == 39 ) q_count++;
//       ++ptr;
//    }
// 
//    if ( q_count == 0 )  return (char *)str; 
// 
//    // allocate mem for original string plus escapes needed.  Note: apache handles mem deallocation.
//    rtn_str = (char*)ap_palloc(pool, strlen(str) + q_count + 1);
// 
//    ptr = (char*)str; 
//    while ( *str ) 
//    {
//       if ( *str == 39 ) 
//       {
//           // rtn_str = strncat(rtn_str, ptr, str - ptr);
//           rtn_str = ap_pstrcat(pool, ap_pstrndup(pool, ptr, str - ptr), rtn_str, "\\", NULL);
//           ptr  = (char *)str;
//       } 
//       ++str;
//    }
//    // rtn_str = strncat(rtn_str, ptr, str - ptr);
//    rtn_str = ap_pstrcat(pool, ap_pstrndup(pool,  ptr, str - ptr), NULL);
//    return rtn_str;
// }

char* trim(ap_pool *pool, const char* const str)
{
   int len;
   char* head_ptr;
   char* tail_ptr;

   if ( str == NULL ) return ap_pstrdup(pool, "");
   len = strlen(str);
   if ( len == 0 ) return (char*)str;

   head_ptr = (char*)str;
   tail_ptr = (char*)(str + len - 1 );

   // find end of leading spaces
   while ( *head_ptr && *head_ptr == 32 ) ++head_ptr;

   if (head_ptr == tail_ptr + 1) return "";

   // find beginning of trailing spaces
   while ( tail_ptr > str && *tail_ptr  == 32 ) --tail_ptr;
   ++tail_ptr;
  
   return ap_pstrndup(pool, head_ptr, tail_ptr - head_ptr);
}
