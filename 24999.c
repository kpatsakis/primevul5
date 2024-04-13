bool Regexp_processor_pcre::exec(Item *item, int offset,
                                uint n_result_offsets_to_convert)
{
  char buff[MAX_FIELD_WIDTH];
  String tmp(buff,sizeof(buff),&my_charset_bin);
  String *res= item->val_str(&tmp);
  if (item->null_value)
    return true;
  return exec(res, offset, n_result_offsets_to_convert);
}