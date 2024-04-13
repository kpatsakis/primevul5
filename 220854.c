repodata_swap_attrs(Repodata *data, Id dest, Id src)
{
  Id *tmpattrs;
  if (!data->attrs || dest == src)
    return;
  if (dest < data->start || dest >= data->end)
    repodata_extend(data, dest);
  if (src < data->start || src >= data->end)
    repodata_extend(data, src);
  tmpattrs = data->attrs[dest - data->start];
  data->attrs[dest - data->start] = data->attrs[src - data->start];
  data->attrs[src - data->start] = tmpattrs;
}