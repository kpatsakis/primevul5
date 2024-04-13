evrid2vrstr(Pool *pool, Id evrid)
{
  const char *p, *evr = pool_id2str(pool, evrid);
  if (!evr)
    return evr;
  for (p = evr; *p >= '0' && *p <= '9'; p++)
    ;
  return p != evr && *p == ':' && p[1] ? p + 1 : evr;
}