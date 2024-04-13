static void end_hquery(struct host_query *hquery, int status)
{
  struct ares_addrinfo_node sentinel;
  struct ares_addrinfo_node *next;
  if (status == ARES_SUCCESS)
    {
      if (!(hquery->hints.ai_flags & ARES_AI_NOSORT))
        {
          sentinel.ai_next = hquery->ai->nodes;
          ares__sortaddrinfo(hquery->channel, &sentinel);
          hquery->ai->nodes = sentinel.ai_next;
        }
      next = hquery->ai->nodes;
      /* Set port into each address (resolved separately). */
      while (next)
        {
          if (next->ai_family == AF_INET)
            {
              (CARES_INADDR_CAST(struct sockaddr_in *, next->ai_addr))->sin_port = htons(hquery->port);
            }
          else
            {
              (CARES_INADDR_CAST(struct sockaddr_in6 *, next->ai_addr))->sin6_port = htons(hquery->port);
            }
          next = next->ai_next;
        }
    }
  else
    {
      /* Clean up what we have collected by so far. */
      ares_freeaddrinfo(hquery->ai);
      hquery->ai = NULL;
    }

  hquery->callback(hquery->arg, status, hquery->timeouts, hquery->ai);
  ares_free(hquery->name);
  ares_free(hquery);
}