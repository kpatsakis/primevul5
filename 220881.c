datamatcher_free(Datamatcher *ma)
{
  if (ma->match)
    ma->match = solv_free((char *)ma->match);
  if ((ma->flags & SEARCH_STRINGMASK) == SEARCH_REGEX && ma->matchdata)
    {
      regfree(ma->matchdata);
      solv_free(ma->matchdata);
    }
  ma->matchdata = 0;
}