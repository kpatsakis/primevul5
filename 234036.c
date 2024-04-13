ds_fgets (FILE *f, dynamic_string *s)
{
  return ds_fgetstr (f, s, '\n');
}