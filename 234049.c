to_ascii_or_error (char *where, uintmax_t n, size_t digits,
		   unsigned logbase,
		   const char *filename, const char *fieldname)
{
  if (to_ascii (where, n, digits, logbase, false))
    {
      field_width_error (filename, fieldname, n, digits, false);
      return 1;
    }
  return 0;
}    