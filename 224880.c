valid_exportstr (v)
     SHELL_VAR *v;
{
  char *s;

  s = v->exportstr;
  if (s == 0)
    {
      internal_error (_("%s has null exportstr"), v->name);
      return (0);
    }
  if (legal_variable_starter ((unsigned char)*s) == 0)
    {
      internal_error (_("invalid character %d in exportstr for %s"), *s, v->name);
      return (0);
    }
  for (s = v->exportstr + 1; s && *s; s++)
    {
      if (*s == '=')
	break;
      if (legal_variable_char ((unsigned char)*s) == 0)
	{
	  internal_error (_("invalid character %d in exportstr for %s"), *s, v->name);
	  return (0);
	}
    }
  if (*s != '=')
    {
      internal_error (_("no `=' in exportstr for %s"), v->name);
      return (0);
    }
  return (1);
}