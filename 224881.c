file_error (filename)
     const char *filename;
{
  report_error ("%s: %s", filename, strerror (errno));
}