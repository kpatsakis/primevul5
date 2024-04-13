url_skip_credentials (const char *url)
{
  /* Look for '@' that comes before terminators, such as '/', '?',
     '#', or ';'.  */
  const char *p = (const char *)strpbrk (url, "@/?#;");
  if (!p || *p != '@')
    return url;
  return p + 1;
}