repodata_extend_block(Repodata *data, Id start, Id num)
{
  if (!num)
    return;
  if (!data->incoreoffset)
    {
      /* this also means that data->attrs is NULL */
      data->incoreoffset = solv_calloc_block(num, sizeof(Id), REPODATA_BLOCK);
      data->start = start;
      data->end = start + num;
      return;
    }
  repodata_extend(data, start);
  if (num > 1)
    repodata_extend(data, start + num - 1);
}