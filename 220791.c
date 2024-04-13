solvid2data(Repodata *data, Id solvid, Id *schemap)
{
  unsigned char *dp = data->incoredata;
  if (!dp)
    return 0;
  if (solvid == SOLVID_META)
    dp += 1;	/* offset of "meta" solvable */
  else if (solvid == SOLVID_POS)
    {
      Pool *pool = data->repo->pool;
      if (data->repo != pool->pos.repo)
	return 0;
      if (data != data->repo->repodata + pool->pos.repodataid)
	return 0;
      dp += pool->pos.dp;
      if (pool->pos.dp != 1)
        {
          *schemap = pool->pos.schema;
          return dp;
	}
    }
  else
    {
      if (solvid < data->start || solvid >= data->end)
	return 0;
      dp += data->incoreoffset[solvid - data->start];
    }
  return data_read_id(dp, schemap);
}