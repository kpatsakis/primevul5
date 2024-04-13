init_property_list(void)
{
  int r;

  PROPERTY_LIST_ADD_PROP("hiragana", CR_Hiragana);
  PROPERTY_LIST_ADD_PROP("katakana", CR_Katakana);
  PropertyInited = 1;

 end:
  return r;
}