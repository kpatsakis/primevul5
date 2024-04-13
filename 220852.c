repodata_freedata(Repodata *data)
{
  int i;

  solv_free(data->keys);

  solv_free(data->schemata);
  solv_free(data->schemadata);
  solv_free(data->schematahash);

  stringpool_free(&data->spool);
  dirpool_free(&data->dirpool);

  solv_free(data->mainschemaoffsets);
  solv_free(data->incoredata);
  solv_free(data->incoreoffset);
  solv_free(data->verticaloffset);

  repopagestore_free(&data->store);

  solv_free(data->vincore);

  if (data->attrs)
    for (i = 0; i < data->end - data->start; i++)
      solv_free(data->attrs[i]);
  solv_free(data->attrs);
  if (data->xattrs)
    for (i = 0; i < data->nxattrs; i++)
      solv_free(data->xattrs[i]);
  solv_free(data->xattrs);

  solv_free(data->attrdata);
  solv_free(data->attriddata);
  solv_free(data->attrnum64data);

  solv_free(data->dircache);

  repodata_free_filelistfilter(data);
}