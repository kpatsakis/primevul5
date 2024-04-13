url_full_path (const struct url *url)
{
  int length = full_path_length (url);
  char *full_path = xmalloc (length + 1);

  full_path_write (url, full_path);
  full_path[length] = '\0';

  return full_path;
}