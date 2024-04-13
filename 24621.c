longlong Item_func_dyncol_check::val_int()
{
  char buff[STRING_BUFFER_USUAL_SIZE];
  String tmp(buff, sizeof(buff), &my_charset_bin);
  DYNAMIC_COLUMN col;
  String *str;
  enum enum_dyncol_func_result rc;

  str= args[0]->val_str(&tmp);
  if (args[0]->null_value)
    goto null;
  col.length= str->length();
  /* We do not change the string, so could do this trick */
  col.str= (char *)str->ptr();
  rc= mariadb_dyncol_check(&col);
  if (rc < 0 && rc != ER_DYNCOL_FORMAT)
  {
    dynamic_column_error_message(rc);
    goto null;
  }
  null_value= FALSE;
  return rc == ER_DYNCOL_OK;

null:
  null_value= TRUE;
  return 0;
}