ds_free (dynamic_string *string)
{
  free (string->ds_string);
}