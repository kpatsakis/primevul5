repodata_set_sourcepkg(Repodata *data, Id solvid, const char *sourcepkg)
{
  Pool *pool = data->repo->pool;
  Solvable *s = pool->solvables + solvid;
  const char *p, *sevr, *sarch, *name, *evr;

  p = strrchr(sourcepkg, '.');
  if (!p || strcmp(p, ".rpm") != 0)
    {
      if (*sourcepkg)
        repodata_set_str(data, solvid, SOLVABLE_SOURCENAME, sourcepkg);
      return;
    }
  p--;
  while (p > sourcepkg && *p != '.')
    p--;
  if (*p != '.' || p == sourcepkg)
    return;
  sarch = p-- + 1;
  while (p > sourcepkg && *p != '-')
    p--;
  if (*p != '-' || p == sourcepkg)
    return;
  p--;
  while (p > sourcepkg && *p != '-')
    p--;
  if (*p != '-' || p == sourcepkg)
    return;
  sevr = p + 1;
  pool = s->repo->pool;

  name = pool_id2str(pool, s->name);
  if (name && !strncmp(sourcepkg, name, sevr - sourcepkg - 1) && name[sevr - sourcepkg - 1] == 0)
    repodata_set_void(data, solvid, SOLVABLE_SOURCENAME);
  else
    repodata_set_id(data, solvid, SOLVABLE_SOURCENAME, pool_strn2id(pool, sourcepkg, sevr - sourcepkg - 1, 1));

  evr = evrid2vrstr(pool, s->evr);
  if (evr && !strncmp(sevr, evr, sarch - sevr - 1) && evr[sarch - sevr - 1] == 0)
    repodata_set_void(data, solvid, SOLVABLE_SOURCEEVR);
  else
    repodata_set_id(data, solvid, SOLVABLE_SOURCEEVR, pool_strn2id(pool, sevr, sarch - sevr - 1, 1));

  if (!strcmp(sarch, "src.rpm"))
    repodata_set_constantid(data, solvid, SOLVABLE_SOURCEARCH, ARCH_SRC);
  else if (!strcmp(sarch, "nosrc.rpm"))
    repodata_set_constantid(data, solvid, SOLVABLE_SOURCEARCH, ARCH_NOSRC);
  else
    repodata_set_constantid(data, solvid, SOLVABLE_SOURCEARCH, pool_strn2id(pool, sarch, strlen(sarch) - 4, 1));
}