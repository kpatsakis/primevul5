static int next_dns_lookup(struct host_query *hquery)
{
  char *s = NULL;
  int is_s_allocated = 0;
  int status;

  /* if next_domain == -1 and as_is_first is true, try hquery->name */
  if (hquery->next_domain == -1)
    {
      if (as_is_first(hquery))
        {
          s = hquery->name;
        }
      hquery->next_domain = 0;
    }

  /* if as_is_first is false, try hquery->name at last */
  if (!s && hquery->next_domain == hquery->channel->ndomains) {
    if (!as_is_first(hquery))
      {
        s = hquery->name;
      }
    hquery->next_domain++;
  }

  if (!s && hquery->next_domain < hquery->channel->ndomains)
    {
      status = ares__cat_domain(
          hquery->name,
          hquery->channel->domains[hquery->next_domain++],
          &s);
      if (status == ARES_SUCCESS)
        {
          is_s_allocated = 1;
        }
    }

  if (s)
    {
      switch (hquery->hints.ai_family)
        {
          case AF_INET:
            hquery->remaining += 1;
            ares_query(hquery->channel, s, C_IN, T_A, host_callback, hquery);
            break;
          case AF_INET6:
            hquery->remaining += 1;
            ares_query(hquery->channel, s, C_IN, T_AAAA, host_callback, hquery);
            break;
          case AF_UNSPEC:
            hquery->remaining += 2;
            ares_query(hquery->channel, s, C_IN, T_A, host_callback, hquery);
            ares_query(hquery->channel, s, C_IN, T_AAAA, host_callback, hquery);
            break;
          default: break;
        }
      if (is_s_allocated)
        {
          ares_free(s);
        }
      return 1;
    }
  else
    {
      assert(!hquery->ai->nodes);
      return 0;
    }
}