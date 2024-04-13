repodata_get_attrp(Repodata *data, Id handle)
{
  if (handle < 0)
    {
      if (handle == SOLVID_META && !data->xattrs)
	{
	  data->xattrs = solv_calloc_block(1, sizeof(Id *), REPODATA_BLOCK);
          data->nxattrs = 2;
	}
      return data->xattrs - handle;
    }
  if (handle < data->start || handle >= data->end)
    repodata_extend(data, handle);
  if (!data->attrs)
    data->attrs = solv_calloc_block(data->end - data->start, sizeof(Id *), REPODATA_BLOCK);
  return data->attrs + (handle - data->start);
}