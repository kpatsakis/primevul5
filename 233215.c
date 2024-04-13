static int fake_addrinfo(const char *name,
                         unsigned short port,
                         const struct ares_addrinfo_hints *hints,
                         struct ares_addrinfo *ai,
                         ares_addrinfo_callback callback,
                         void *arg)
{
  struct ares_addrinfo_cname *cname;
  struct ares_addrinfo_node *node;
  ares_sockaddr addr;
  size_t addrlen;
  int result = 0;
  int family = hints->ai_family;
  if (family == AF_INET || family == AF_INET6 || family == AF_UNSPEC)
    {
      /* It only looks like an IP address if it's all numbers and dots. */
      int numdots = 0, valid = 1;
      const char *p;
      for (p = name; *p; p++)
        {
          if (!ISDIGIT(*p) && *p != '.')
            {
              valid = 0;
              break;
            }
          else if (*p == '.')
            {
              numdots++;
            }
        }

      memset(&addr, 0, sizeof(addr));

      /* if we don't have 3 dots, it is illegal
       * (although inet_pton doesn't think so).
       */
      if (numdots != 3 || !valid)
        result = 0;
      else
        result =
            (ares_inet_pton(AF_INET, name, &addr.sa4.sin_addr) < 1 ? 0 : 1);

      if (result)
        {
          family = addr.sa.sa_family = AF_INET;
          addr.sa4.sin_port = htons(port);
          addrlen = sizeof(addr.sa4);
        }
    }

  if (family == AF_INET6 || family == AF_UNSPEC)
    {
      result =
          (ares_inet_pton(AF_INET6, name, &addr.sa6.sin6_addr) < 1 ? 0 : 1);
      addr.sa6.sin6_family = AF_INET6;
      addr.sa6.sin6_port = htons(port);
      addrlen = sizeof(addr.sa6);
    }

  if (!result)
    return 0;

  node = ares__malloc_addrinfo_node();
  if (!node)
    {
      ares_freeaddrinfo(ai);
      callback(arg, ARES_ENOMEM, 0, NULL);
      return 1;
    }

  ai->nodes = node;

  node->ai_addr = ares_malloc(addrlen);
  if (!node->ai_addr)
    {
      ares_freeaddrinfo(ai);
      callback(arg, ARES_ENOMEM, 0, NULL);
      return 1;
    }

  node->ai_addrlen = (unsigned int)addrlen;
  node->ai_family = addr.sa.sa_family;
  if (addr.sa.sa_family == AF_INET)
    memcpy(node->ai_addr, &addr.sa4, sizeof(addr.sa4));
  else
    memcpy(node->ai_addr, &addr.sa6, sizeof(addr.sa6));

  if (hints->ai_flags & ARES_AI_CANONNAME)
    {
      cname = ares__append_addrinfo_cname(&ai->cnames);
      if (!cname)
        {
          ares_freeaddrinfo(ai);
          callback(arg, ARES_ENOMEM, 0, NULL);
          return 1;
        }

      /* Duplicate the name, to avoid a constness violation. */
      cname->name = ares_strdup(name);
      if (!cname->name)
        {
          ares_freeaddrinfo(ai);
          callback(arg, ARES_ENOMEM, 0, NULL);
          return 1;
        }
    }

  callback(arg, ARES_SUCCESS, 0, ai);
  return 1;
}