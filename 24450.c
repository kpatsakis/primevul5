void trim_whitespace(CHARSET_INFO *cs, LEX_STRING *str, uint *prefix_length)
{
  /*
    TODO:
    This code assumes that there are no multi-bytes characters
    that can be considered white-space.
  */

  *prefix_length= 0;
  while ((str->length > 0) && (my_isspace(cs, str->str[0])))
  {
    (*prefix_length)++;
    str->length --;
    str->str ++;
  }

  /*
    FIXME:
    Also, parsing backward is not safe with multi bytes characters
  */
  while ((str->length > 0) && (my_isspace(cs, str->str[str->length-1])))
  {
    str->length --;
  }
}