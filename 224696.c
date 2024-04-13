is_dirname (pathname)
     char *pathname;
{
  char *temp;
  int ret;

  temp = search_for_command (pathname, 0);
  ret = (temp ? file_isdir (temp) : file_isdir (pathname));
  free (temp);
  return ret;
}