struct ares_addrinfo *ares__malloc_addrinfo()
{
  struct ares_addrinfo *ai = ares_malloc(sizeof(struct ares_addrinfo));
  if (!ai)
    return NULL;

  *ai = empty_addrinfo;
  return ai;
}