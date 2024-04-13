repodata_set_location(Repodata *data, Id solvid, int medianr, const char *dir, const char *file)
{
  Pool *pool = data->repo->pool;
  Solvable *s;
  const char *str, *fp;
  int l = 0;

  if (medianr)
    repodata_set_constant(data, solvid, SOLVABLE_MEDIANR, medianr);
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
  s = pool->solvables + solvid;
  if (dir && l)
    {
      str = pool_id2str(pool, s->arch);
      if (!strncmp(dir, str, l) && !str[l])
	repodata_set_void(data, solvid, SOLVABLE_MEDIADIR);
      else
	repodata_set_strn(data, solvid, SOLVABLE_MEDIADIR, dir, l);
    }
  fp = file;
  str = pool_id2str(pool, s->name);
  l = strlen(str);
  if ((!l || !strncmp(fp, str, l)) && fp[l] == '-')
    {
      fp += l + 1;
      str = evrid2vrstr(pool, s->evr);
      l = strlen(str);
      if ((!l || !strncmp(fp, str, l)) && fp[l] == '.')
	{
	  fp += l + 1;
	  str = pool_id2str(pool, s->arch);
	  l = strlen(str);
	  if ((!l || !strncmp(fp, str, l)) && !strcmp(fp + l, ".rpm"))
	    {
	      repodata_set_void(data, solvid, SOLVABLE_MEDIAFILE);
	      return;
	    }
	}
    }
  repodata_set_str(data, solvid, SOLVABLE_MEDIAFILE, file);
}