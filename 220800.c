repodata_create_stubs(Repodata *data)
{
  Repo *repo = data->repo;
  Pool *pool = repo->pool;
  Repodata *sdata;
  int *stubdataids;
  Dataiterator di;
  Id xkeyname = 0;
  int i, cnt = 0;

  dataiterator_init(&di, pool, repo, SOLVID_META, REPOSITORY_EXTERNAL, 0, 0);
  while (dataiterator_step(&di))
    if (di.data == data)
      cnt++;
  dataiterator_free(&di);
  if (!cnt)
    return data;
  stubdataids = solv_calloc(cnt, sizeof(*stubdataids));
  for (i = 0; i < cnt; i++)
    {
      sdata = repodata_add_stub(&data);
      stubdataids[i] = sdata - repo->repodata;
    }
  i = 0;
  dataiterator_init(&di, pool, repo, SOLVID_META, REPOSITORY_EXTERNAL, 0, 0);
  sdata = 0;
  while (dataiterator_step(&di))
    {
      if (di.data != data)
	continue;
      if (di.key->name == REPOSITORY_EXTERNAL && !di.nparents)
	{
	  dataiterator_entersub(&di);
	  sdata = repo->repodata + stubdataids[i++];
	  xkeyname = 0;
	  continue;
	}
      repodata_set_kv(sdata, SOLVID_META, di.key->name, di.key->type, &di.kv);
      if (di.key->name == REPOSITORY_KEYS && di.key->type == REPOKEY_TYPE_IDARRAY)
	{
	  if (!xkeyname)
	    {
	      if (!di.kv.eof)
		xkeyname = di.kv.id;
	    }
	  else
	    {
	      repodata_add_stubkey(sdata, xkeyname, di.kv.id);
	      if (xkeyname == SOLVABLE_FILELIST)
	        repodata_set_filelisttype(sdata, REPODATA_FILELIST_EXTENSION);
	      xkeyname = 0;
	    }
	}
    }
  dataiterator_free(&di);
  for (i = 0; i < cnt; i++)
    repodata_internalize(repo->repodata + stubdataids[i]);
  solv_free(stubdataids);
  return data;
}