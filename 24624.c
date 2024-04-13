bool Regexp_processor_pcre::compile(Item *item, bool send_error)
{
  char buff[MAX_FIELD_WIDTH];
  String tmp(buff, sizeof(buff), &my_charset_bin);
  String *pattern= item->val_str(&tmp);
  if (unlikely(item->null_value) || (unlikely(compile(pattern, send_error))))
    return true;
  return false;
}