uri_merge (const char *base, const char *link)
{
  int linklength;
  const char *end;
  char *merge;

  if (url_has_scheme (link))
    return xstrdup (link);

  /* We may not examine BASE past END. */
  end = path_end (base);
  linklength = strlen (link);

  if (!*link)
    {
      /* Empty LINK points back to BASE, query string and all. */
      return xstrdup (base);
    }
  else if (*link == '?')
    {
      /* LINK points to the same location, but changes the query
         string.  Examples: */
      /* uri_merge("path",         "?new") -> "path?new"     */
      /* uri_merge("path?foo",     "?new") -> "path?new"     */
      /* uri_merge("path?foo#bar", "?new") -> "path?new"     */
      /* uri_merge("path#foo",     "?new") -> "path?new"     */
      int baselength = end - base;
      merge = xmalloc (baselength + linklength + 1);
      memcpy (merge, base, baselength);
      memcpy (merge + baselength, link, linklength);
      merge[baselength + linklength] = '\0';
    }
  else if (*link == '#')
    {
      /* uri_merge("path",         "#new") -> "path#new"     */
      /* uri_merge("path#foo",     "#new") -> "path#new"     */
      /* uri_merge("path?foo",     "#new") -> "path?foo#new" */
      /* uri_merge("path?foo#bar", "#new") -> "path?foo#new" */
      int baselength;
      const char *end1 = strchr (base, '#');
      if (!end1)
        end1 = base + strlen (base);
      baselength = end1 - base;
      merge = xmalloc (baselength + linklength + 1);
      memcpy (merge, base, baselength);
      memcpy (merge + baselength, link, linklength);
      merge[baselength + linklength] = '\0';
    }
  else if (*link == '/' && *(link + 1) == '/')
    {
      /* LINK begins with "//" and so is a net path: we need to
         replace everything after (and including) the double slash
         with LINK. */

      /* uri_merge("foo", "//new/bar")            -> "//new/bar"      */
      /* uri_merge("//old/foo", "//new/bar")      -> "//new/bar"      */
      /* uri_merge("http://old/foo", "//new/bar") -> "http://new/bar" */

      int span;
      const char *slash;
      const char *start_insert;

      /* Look for first slash. */
      slash = memchr (base, '/', end - base);
      /* If found slash and it is a double slash, then replace
         from this point, else default to replacing from the
         beginning.  */
      if (slash && *(slash + 1) == '/')
        start_insert = slash;
      else
        start_insert = base;

      span = start_insert - base;
      merge = xmalloc (span + linklength + 1);
      if (span)
        memcpy (merge, base, span);
      memcpy (merge + span, link, linklength);
      merge[span + linklength] = '\0';
    }
  else if (*link == '/')
    {
      /* LINK is an absolute path: we need to replace everything
         after (and including) the FIRST slash with LINK.

         So, if BASE is "http://host/whatever/foo/bar", and LINK is
         "/qux/xyzzy", our result should be
         "http://host/qux/xyzzy".  */
      int span;
      const char *slash;
      const char *start_insert = NULL; /* for gcc to shut up. */
      const char *pos = base;
      bool seen_slash_slash = false;
      /* We're looking for the first slash, but want to ignore
         double slash. */
    again:
      slash = memchr (pos, '/', end - pos);
      if (slash && !seen_slash_slash)
        if (*(slash + 1) == '/')
          {
            pos = slash + 2;
            seen_slash_slash = true;
            goto again;
          }

      /* At this point, SLASH is the location of the first / after
         "//", or the first slash altogether.  START_INSERT is the
         pointer to the location where LINK will be inserted.  When
         examining the last two examples, keep in mind that LINK
         begins with '/'. */

      if (!slash && !seen_slash_slash)
        /* example: "foo" */
        /*           ^    */
        start_insert = base;
      else if (!slash && seen_slash_slash)
        /* example: "http://foo" */
        /*                     ^ */
        start_insert = end;
      else if (slash && !seen_slash_slash)
        /* example: "foo/bar" */
        /*           ^        */
        start_insert = base;
      else if (slash && seen_slash_slash)
        /* example: "http://something/" */
        /*                           ^  */
        start_insert = slash;

      span = start_insert - base;
      merge = xmalloc (span + linklength + 1);
      if (span)
        memcpy (merge, base, span);
      memcpy (merge + span, link, linklength);
      merge[span + linklength] = '\0';
    }
  else
    {
      /* LINK is a relative URL: we need to replace everything
         after last slash (possibly empty) with LINK.

         So, if BASE is "whatever/foo/bar", and LINK is "qux/xyzzy",
         our result should be "whatever/foo/qux/xyzzy".  */
      bool need_explicit_slash = false;
      int span;
      const char *start_insert;
      const char *last_slash = find_last_char (base, end, '/');
      if (!last_slash)
        {
          /* No slash found at all.  Replace what we have with LINK. */
          start_insert = base;
        }
      else if (last_slash && last_slash >= base + 2
               && last_slash[-2] == ':' && last_slash[-1] == '/')
        {
          /* example: http://host"  */
          /*                      ^ */
          start_insert = end + 1;
          need_explicit_slash = true;
        }
      else
        {
          /* example: "whatever/foo/bar" */
          /*                        ^    */
          start_insert = last_slash + 1;
        }

      span = start_insert - base;
      merge = xmalloc (span + linklength + 1);
      if (span)
        memcpy (merge, base, span);
      if (need_explicit_slash)
        merge[span - 1] = '/';
      memcpy (merge + span, link, linklength);
      merge[span + linklength] = '\0';
    }

  return merge;
}