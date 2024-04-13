repodata_set_deltalocation(Repodata *data, Id handle, int medianr, const char *dir, const char *file)
{
  int l = 0;
  const char *evr, *suf, *s;

  if (!dir)
    {
      if ((dir = strrchr(file, '/')) != 0)
	{
          l = dir - file;
	  dir = file;
	  file = dir + l + 1;
	  if (!l)
	    l++;
	}
    }
  else
    l = strlen(dir);
  if (l >= 2 && dir[0] == '.' && dir[1] == '/' && (l == 2 || dir[2] != '/'))
    {
      dir += 2;
      l -= 2;
    }
  if (l == 1 && dir[0] == '.')
    l = 0;
  if (dir && l)
    repodata_set_poolstrn(data, handle, DELTA_LOCATION_DIR, dir, l);
  evr = strchr(file, '-');
  if (evr)
    {
      for (s = evr - 1; s > file; s--)
	if (*s == '-')
	  {
	    evr = s;
	    break;
	  }
    }
  suf = strrchr(file, '.');
  if (suf)
    {
      for (s = suf - 1; s > file; s--)
	if (*s == '.')
	  {
	    suf = s;
	    break;
	  }
      if (!strcmp(suf, ".delta.rpm") || !strcmp(suf, ".patch.rpm"))
	{
	  /* We accept one more item as suffix.  */
	  for (s = suf - 1; s > file; s--)
	    if (*s == '.')
	      {
		suf = s;
	        break;
	      }
	}
    }
  if (!evr)
    suf = 0;
  if (suf && evr && suf < evr)
    suf = 0;
  repodata_set_poolstrn(data, handle, DELTA_LOCATION_NAME, file, evr ? evr - file : strlen(file));
  if (evr)
    repodata_set_poolstrn(data, handle, DELTA_LOCATION_EVR, evr + 1, suf ? suf - evr - 1: strlen(evr + 1));
  if (suf)
    repodata_set_poolstr(data, handle, DELTA_LOCATION_SUFFIX, suf + 1);
}