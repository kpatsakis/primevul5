ds_fgetname (FILE *f, dynamic_string *s)
{
  return ds_fgetstr (f, s, '\0');
}