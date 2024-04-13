repodata_new_handle(Repodata *data)
{
  if (!data->nxattrs)
    {
      data->xattrs = solv_calloc_block(1, sizeof(Id *), REPODATA_BLOCK);
      data->nxattrs = 2;	/* -1: SOLVID_META */
    }
  data->xattrs = solv_extend(data->xattrs, data->nxattrs, 1, sizeof(Id *), REPODATA_BLOCK);
  data->xattrs[data->nxattrs] = 0;
  return -(data->nxattrs++);
}