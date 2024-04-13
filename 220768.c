datamatcher_init(Datamatcher *ma, const char *match, int flags)
{
  match = match ? solv_strdup(match) : 0;
  ma->match = match;
  ma->flags = flags;
  ma->error = 0;
  ma->matchdata = 0;
  if ((flags & SEARCH_STRINGMASK) == SEARCH_REGEX)
    {
      ma->matchdata = solv_calloc(1, sizeof(regex_t));
      ma->error = regcomp((regex_t *)ma->matchdata, match, REG_EXTENDED | REG_NOSUB | REG_NEWLINE | ((flags & SEARCH_NOCASE) ? REG_ICASE : 0));
      if (ma->error)
	{
	  solv_free(ma->matchdata);
	  ma->flags = (flags & ~SEARCH_STRINGMASK) | SEARCH_ERROR;
	}
    }
  if ((flags & SEARCH_FILES) != 0 && match)
    {
      /* prepare basename check */
      if ((flags & SEARCH_STRINGMASK) == SEARCH_STRING || (flags & SEARCH_STRINGMASK) == SEARCH_STRINGEND)
	{
	  const char *p = strrchr(match, '/');
	  ma->matchdata = (void *)(p ? p + 1 : match);
	}
      else if ((flags & SEARCH_STRINGMASK) == SEARCH_GLOB)
	{
	  const char *p;
	  for (p = match + strlen(match) - 1; p >= match; p--)
	    if (*p == '[' || *p == ']' || *p == '*' || *p == '?' || *p == '/')
	      break;
	  ma->matchdata = (void *)(p + 1);
	}
    }
  return ma->error;
}