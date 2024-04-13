struct ares_addrinfo_node *ares__append_addrinfo_node(struct ares_addrinfo_node **head)
{
  struct ares_addrinfo_node *tail = ares__malloc_addrinfo_node();
  struct ares_addrinfo_node *last = *head;
  if (!last)
    {
      *head = tail;
      return tail;
    }

  while (last->ai_next)
    {
      last = last->ai_next;
    }

  last->ai_next = tail;
  return tail;
}