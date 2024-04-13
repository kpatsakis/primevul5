url_escape_dir (const char *dir)
{
  char *newdir = url_escape_1 (dir, urlchr_unsafe | urlchr_reserved, 1);
  if (newdir == dir)
    return (char *)dir;

  unescape_single_char (newdir, '/');
  return newdir;
}