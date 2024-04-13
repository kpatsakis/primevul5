static inline int str2in_method(char *optarg) {
    if (optarg) {
#ifdef PROC_NET_DEV
        if (!strcasecmp(optarg,"proc")) return PROC_IN;
#endif
#ifdef NETSTAT
        if (!strcasecmp(optarg,"netstat")) return NETSTAT_IN;
#endif
#ifdef LIBSTATGRAB
        if (!strcasecmp(optarg,"libstat") || !strcasecmp(optarg,"statgrab") || !strcasecmp(optarg,"libstatgrab")) return LIBSTAT_IN;
		  if (!strcasecmp(optarg,"libstatdisk")) return LIBSTATDISK_IN;
#endif
#ifdef GETIFADDRS
        if (!strcasecmp(optarg,"getifaddrs")) return GETIFADDRS_IN;
#endif
#if DEVSTAT_IN
		  if (!strcasecmp(optarg,"devstat")) return DEVSTAT_IN;
#endif
#ifdef SYSCTL
        if (!strcasecmp(optarg,"sysctl")) return SYSCTL_IN;
#endif
#if SYSCTLDISK_IN
		  if (!strcasecmp(optarg,"sysctldisk")) return SYSCTLDISK_IN;
#endif
#ifdef PROC_DISKSTATS
		  if (!strcasecmp(optarg,"disk")) return DISKLINUX_IN;
#endif		  
#ifdef WIN32
		  if (!strcasecmp(optarg,"win32")) return WIN32_IN;
#endif			  
#ifdef HAVE_LIBKSTAT
			if (!strcasecmp(optarg,"kstat")) return KSTAT_IN;
			if (!strcasecmp(optarg,"kstatdisk")) return KSTATDISK_IN;
#endif 
#if IOSERVICE_IN
			if (!strcasecmp(optarg,"ioservice")) return IOSERVICE_IN;
#endif
    }
    return -1;
}