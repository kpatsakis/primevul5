void ares__addrinfo_cat_nodes(struct ares_addrinfo_node **head,
                              struct ares_addrinfo_node *tail)
{
  struct ares_addrinfo_node *last = *head;
  if (!last)
    {
      *head = tail;
      return;
    }

  while (last->ai_next)
    {
      last = last->ai_next;
    }

  last->ai_next = tail;
}