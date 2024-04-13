static unsigned short lookup_service(const char *service, int flags)
{
  const char *proto;
  struct servent *sep;
#ifdef HAVE_GETSERVBYNAME_R
  struct servent se;
  char tmpbuf[4096];
#endif

  if (service)
    {
      if (flags & ARES_NI_UDP)
        proto = "udp";
      else if (flags & ARES_NI_SCTP)
        proto = "sctp";
      else if (flags & ARES_NI_DCCP)
        proto = "dccp";
      else
        proto = "tcp";
#ifdef HAVE_GETSERVBYNAME_R
      memset(&se, 0, sizeof(se));
      sep = &se;
      memset(tmpbuf, 0, sizeof(tmpbuf));
#if GETSERVBYNAME_R_ARGS == 6
      if (getservbyname_r(service, proto, &se, (void *)tmpbuf, sizeof(tmpbuf),
                          &sep) != 0)
        sep = NULL; /* LCOV_EXCL_LINE: buffer large so this never fails */
#elif GETSERVBYNAME_R_ARGS == 5
      sep =
          getservbyname_r(service, proto, &se, (void *)tmpbuf, sizeof(tmpbuf));
#elif GETSERVBYNAME_R_ARGS == 4
      if (getservbyname_r(service, proto, &se, (void *)tmpbuf) != 0)
        sep = NULL;
#else
      /* Lets just hope the OS uses TLS! */
      sep = getservbyname(service, proto);
#endif
#else
        /* Lets just hope the OS uses TLS! */
#if (defined(NETWARE) && !defined(__NOVELL_LIBC__))
      sep = getservbyname(service, (char *)proto);
#else
      sep = getservbyname(service, proto);
#endif
#endif
      return (sep ? ntohs((unsigned short)sep->s_port) : 0);
    }
  return 0;
}