repodata_extend(Repodata *data, Id p)
{
  if (data->start == data->end)
    data->start = data->end = p;
  if (p >= data->end)
    {
      int old = data->end - data->start;
      int new = p - data->end + 1;
      if (data->attrs)
	{
	  data->attrs = solv_extend(data->attrs, old, new, sizeof(Id *), REPODATA_BLOCK);
	  memset(data->attrs + old, 0, new * sizeof(Id *));
	}
      data->incoreoffset = solv_extend(data->incoreoffset, old, new, sizeof(Id), REPODATA_BLOCK);
      memset(data->incoreoffset + old, 0, new * sizeof(Id));
      data->end = p + 1;
    }
  if (p < data->start)
    {
      int old = data->end - data->start;
      int new = data->start - p;
      if (data->attrs)
	{
	  data->attrs = solv_extend_resize(data->attrs, old + new, sizeof(Id *), REPODATA_BLOCK);
	  memmove(data->attrs + new, data->attrs, old * sizeof(Id *));
	  memset(data->attrs, 0, new * sizeof(Id *));
	}
      data->incoreoffset = solv_extend_resize(data->incoreoffset, old + new, sizeof(Id), REPODATA_BLOCK);
      memmove(data->incoreoffset + new, data->incoreoffset, old * sizeof(Id));
      memset(data->incoreoffset, 0, new * sizeof(Id));
      data->start = p;
    }
}