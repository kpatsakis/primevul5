kwsexec (kwset_t kwset, char const *text, size_t size,
         struct kwsmatch *kwsmatch)
{
  if (kwset->words == 1)
    {
      size_t ret = bmexec (kwset, text, size);
      if (ret != (size_t) -1)
        {
          kwsmatch->index = 0;
          kwsmatch->offset[0] = ret;
          kwsmatch->size[0] = kwset->mind;
        }
      return ret;
    }
  else
    return cwexec (kwset, text, size, kwsmatch);
}