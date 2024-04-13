struct ares_addrinfo_cname *ares__malloc_addrinfo_cname()
{
  struct ares_addrinfo_cname *cname = ares_malloc(sizeof(struct ares_addrinfo_cname));
  if (!cname)
    return NULL;

  *cname = empty_addrinfo_cname;
  return cname;
}