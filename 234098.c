assign_string (char **pvar, char *value)
{
  char *p = xrealloc (*pvar, strlen (value) + 1);
  strcpy (p, value);
  *pvar = p;
}