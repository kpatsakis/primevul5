struct ares_addrinfo_node *ares__malloc_addrinfo_node()
{
  struct ares_addrinfo_node *node =
      ares_malloc(sizeof(struct ares_addrinfo_node));
  if (!node)
    return NULL;

  *node = empty_addrinfo_node;
  return node;
}