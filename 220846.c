datamatcher_match(Datamatcher *ma, const char *str)
{
  int l;
  switch ((ma->flags & SEARCH_STRINGMASK))
    {
    case SEARCH_SUBSTRING:
      if (ma->flags & SEARCH_NOCASE)
	return strcasestr(str, ma->match) != 0;
      else
	return strstr(str, ma->match) != 0;
    case SEARCH_STRING:
      if (ma->flags & SEARCH_NOCASE)
	return !strcasecmp(ma->match, str);
      else
	return !strcmp(ma->match, str);
    case SEARCH_STRINGSTART:
      if (ma->flags & SEARCH_NOCASE)
        return !strncasecmp(ma->match, str, strlen(ma->match));
      else
        return !strncmp(ma->match, str, strlen(ma->match));
    case SEARCH_STRINGEND:
      l = strlen(str) - strlen(ma->match);
      if (l < 0)
	return 0;
      if (ma->flags & SEARCH_NOCASE)
	return !strcasecmp(ma->match, str + l);
      else
	return !strcmp(ma->match, str + l);
    case SEARCH_GLOB:
      return !fnmatch(ma->match, str, (ma->flags & SEARCH_NOCASE) ? FNM_CASEFOLD : 0);
    case SEARCH_REGEX:
      return !regexec((const regex_t *)ma->matchdata, str, 0, NULL, 0);
    default:
      return 0;
    }
}