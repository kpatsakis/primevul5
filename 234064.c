field_width_warning (const char *filename, const char *fieldname)
{
  if (warn_option & CPIO_WARN_TRUNCATE)
    error (0, 0, _("%s: truncating %s"), filename, fieldname);
}