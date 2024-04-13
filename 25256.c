longlong Item_func_dyncol_exists::val_int()
{
  char buff[STRING_BUFFER_USUAL_SIZE], nmstrbuf[11];
  String tmp(buff, sizeof(buff), &my_charset_bin),
         nmbuf(nmstrbuf, sizeof(nmstrbuf), system_charset_info);
  DYNAMIC_COLUMN col;
  String *str;
  LEX_STRING buf, *name= NULL;
  ulonglong num= 0;
  enum enum_dyncol_func_result rc;

  if (args[1]->result_type() == INT_RESULT)
    num= args[1]->val_int();
  else
  {
    String *nm= args[1]->val_str(&nmbuf);
    if (!nm || args[1]->null_value)
    {
      null_value= 1;
      return 1;
    }
    if (my_charset_same(nm->charset(), DYNCOL_UTF))
    {
      buf.str= (char *) nm->ptr();
      buf.length= nm->length();
    }
    else
    {
      uint strlen= nm->length() * DYNCOL_UTF->mbmaxlen + 1;
      uint dummy_errors;
      buf.str= (char *) current_thd->alloc(strlen);
      if (buf.str)
      {
        buf.length=
          copy_and_convert(buf.str, strlen, DYNCOL_UTF,
                           nm->ptr(), nm->length(), nm->charset(),
                           &dummy_errors);
      }
      else
        buf.length= 0;
    }
    name= &buf;
  }
  str= args[0]->val_str(&tmp);
  if (args[0]->null_value || args[1]->null_value || num > UINT_MAX16)
    goto null;
  col.length= str->length();
  /* We do not change the string, so could do this trick */
  col.str= (char *)str->ptr();
  rc= ((name == NULL) ?
       mariadb_dyncol_exists_num(&col, (uint) num) :
       mariadb_dyncol_exists_named(&col, name));
  if (rc < 0)
  {
    dynamic_column_error_message(rc);
    goto null;
  }
  null_value= FALSE;
  return rc == ER_DYNCOL_YES;

null:
  null_value= TRUE;
  return 0;
}