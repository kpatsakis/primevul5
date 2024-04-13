void ares_getaddrinfo(ares_channel channel,
                      const char* name, const char* service,
                      const struct ares_addrinfo_hints* hints,
                      ares_addrinfo_callback callback, void* arg)
{
  struct host_query *hquery;
  unsigned short port = 0;
  int family;
  struct ares_addrinfo *ai;

  if (!hints)
    {
      hints = &default_hints;
    }

  family = hints->ai_family;

  /* Right now we only know how to look up Internet addresses
     and unspec means try both basically. */
  if (family != AF_INET &&
      family != AF_INET6 &&
      family != AF_UNSPEC)
    {
      callback(arg, ARES_ENOTIMP, 0, NULL);
      return;
    }

  if (ares__is_onion_domain(name))
    {
      callback(arg, ARES_ENOTFOUND, 0, NULL);
      return;
    }

  if (service)
    {
      if (hints->ai_flags & ARES_AI_NUMERICSERV)
        {
          port = (unsigned short)strtoul(service, NULL, 0);
          if (!port)
            {
              callback(arg, ARES_ESERVICE, 0, NULL);
              return;
            }
        }
      else
        {
          port = lookup_service(service, 0);
          if (!port)
            {
              port = (unsigned short)strtoul(service, NULL, 0);
              if (!port)
                {
                  callback(arg, ARES_ESERVICE, 0, NULL);
                  return;
                }
            }
        }
    }

  ai = ares__malloc_addrinfo();
  if (!ai)
    {
      callback(arg, ARES_ENOMEM, 0, NULL);
      return;
    }

  if (fake_addrinfo(name, port, hints, ai, callback, arg))
    {
      return;
    }

  /* Allocate and fill in the host query structure. */
  hquery = ares_malloc(sizeof(struct host_query));
  if (!hquery)
    {
      ares_freeaddrinfo(ai);
      callback(arg, ARES_ENOMEM, 0, NULL);
      return;
    }

  hquery->name = ares_strdup(name);
  if (!hquery->name)
    {
      ares_free(hquery);
      ares_freeaddrinfo(ai);
      callback(arg, ARES_ENOMEM, 0, NULL);
      return;
    }

  hquery->port = port;
  hquery->channel = channel;
  hquery->hints = *hints;
  hquery->sent_family = -1; /* nothing is sent yet */
  hquery->callback = callback;
  hquery->arg = arg;
  hquery->remaining_lookups = channel->lookups;
  hquery->timeouts = 0;
  hquery->ai = ai;
  hquery->next_domain = -1;
  hquery->remaining = 0;

  /* Start performing lookups according to channel->lookups. */
  next_lookup(hquery, ARES_ECONNREFUSED /* initial error code */);
}