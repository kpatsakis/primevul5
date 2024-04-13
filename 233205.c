struct ares_addrinfo_cname *ares__append_addrinfo_cname(struct ares_addrinfo_cname **head)
{
  struct ares_addrinfo_cname *tail = ares__malloc_addrinfo_cname();
  struct ares_addrinfo_cname *last = *head;
  if (!last)
    {
      *head = tail;
      return tail;
    }

  while (last->next)
    {
      last = last->next;
    }

  last->next = tail;
  return tail;
}