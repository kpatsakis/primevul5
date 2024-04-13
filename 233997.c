ps (char *path)
{
  char *copy = xstrdup (path);
  path_simplify (copy);
  return copy;
}