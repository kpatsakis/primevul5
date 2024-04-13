find_special_var (name)
     const char *name;
{
  register int i, r;

  for (i = 0; special_vars[i].name; i++)
    {
      r = special_vars[i].name[0] - name[0];
      if (r == 0)
	r = strcmp (special_vars[i].name, name);
      if (r == 0)
	return i;
      else if (r > 0)
	/* Can't match any of rest of elements in sorted list.  Take this out
	   if it causes problems in certain environments. */
	break;
    }
  return -1;
}