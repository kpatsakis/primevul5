datamatcher_checkbasename(Datamatcher *ma, const char *basename)
{
  int l;
  const char *match = ma->matchdata;
  if (!match)
    return 1;
  switch (ma->flags & SEARCH_STRINGMASK)
    {
    case SEARCH_STRING:
      break;
    case SEARCH_STRINGEND:
      if (match != ma->match)
	break;		/* had slash, do exact match on basename */
      /* FALLTHROUGH */
    case SEARCH_GLOB:
      /* check if the basename ends with match */
      l = strlen(basename) - strlen(match);
      if (l < 0)
	return 0;
      basename += l;
      break;
    default:
      return 1;	/* maybe matches */
    }
  if ((ma->flags & SEARCH_NOCASE) != 0)
    return !strcasecmp(match, basename);
  else
    return !strcmp(match, basename);
}