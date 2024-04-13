repodata_shrink(Repodata *data, int end)
{
  int i;

  if (data->end <= end)
    return;
  if (data->start >= end)
    {
      if (data->attrs)
	{
	  for (i = 0; i < data->end - data->start; i++)
	    solv_free(data->attrs[i]);
          data->attrs = solv_free(data->attrs);
	}
      data->incoreoffset = solv_free(data->incoreoffset);
      data->start = data->end = 0;
      return;
    }
  if (data->attrs)
    {
      for (i = end; i < data->end; i++)
	solv_free(data->attrs[i - data->start]);
      data->attrs = solv_extend_resize(data->attrs, end - data->start, sizeof(Id *), REPODATA_BLOCK);
    }
  if (data->incoreoffset)
    data->incoreoffset = solv_extend_resize(data->incoreoffset, end - data->start, sizeof(Id), REPODATA_BLOCK);
  data->end = end;
}