void ares__addrinfo_cat_cnames(struct ares_addrinfo_cname **head,
                               struct ares_addrinfo_cname *tail)
{
  struct ares_addrinfo_cname *last = *head;
  if (!last)
    {
      *head = tail;
      return;
    }

  while (last->next)
    {
      last = last->next;
    }

  last->next = tail;
}