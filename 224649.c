all_variables_matching_prefix (prefix)
     const char *prefix;
{
  SHELL_VAR **varlist;
  char **rlist;
  int vind, rind, plen;

  plen = STRLEN (prefix);
  varlist = all_visible_variables ();
  for (vind = 0; varlist && varlist[vind]; vind++)
    ;
  if (varlist == 0 || vind == 0)
    return ((char **)NULL);
  rlist = strvec_create (vind + 1);
  for (vind = rind = 0; varlist[vind]; vind++)
    {
      if (plen == 0 || STREQN (prefix, varlist[vind]->name, plen))
	rlist[rind++] = savestring (varlist[vind]->name);
    }
  rlist[rind] = (char *)0;
  free (varlist);

  return rlist;
}