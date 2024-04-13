print_build_options(void)
{
	const char** m;
	const char *evnm="event", *evsys="", *evmethod="";
	time_t t;
	struct timeval now;
	struct ub_event_base* base;
	printf("Version %s\n\n", PACKAGE_VERSION);
	printf("Configure line: %s\n", CONFCMDLINE);
	base = ub_default_event_base(0,&t,&now);
	ub_get_event_sys(base, &evnm, &evsys, &evmethod);
	printf("Linked libs: %s %s (it uses %s), %s\n",
		evnm, evsys, evmethod,
#ifdef HAVE_SSL
#  ifdef SSLEAY_VERSION
		SSLeay_version(SSLEAY_VERSION)
#  else
		OpenSSL_version(OPENSSL_VERSION)
#  endif
#elif defined(HAVE_NSS)
		NSS_GetVersion()
#elif defined(HAVE_NETTLE)
		"nettle"
#endif
		);
	printf("Linked modules:");
	for(m = module_list_avail(); *m; m++)
		printf(" %s", *m);
	printf("\n");
#ifdef USE_DNSCRYPT
	printf("DNSCrypt feature available\n");
#endif
#ifdef USE_TCP_FASTOPEN
	printf("TCP Fastopen feature available\n");
#endif
	ub_event_base_free(base);
	printf("\nBSD licensed, see LICENSE in source package for details.\n");
	printf("Report bugs to %s\n", PACKAGE_BUGREPORT);
}