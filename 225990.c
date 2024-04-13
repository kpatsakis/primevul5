static inline int str2out_method(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"plain")) return PLAIN_OUT;
#ifdef HAVE_CURSES
            else
        if (!strcasecmp(optarg,"curses")) return CURSES_OUT;
	    else
        if (!strcasecmp(optarg,"curses2")) return CURSES2_OUT;
#endif
#ifdef CSV
            else
        if (!strcasecmp(optarg,"csv")) return CSV_OUT;
#endif
#ifdef HTML
        else
        if (!strcasecmp(optarg,"html")) return HTML_OUT;
#endif
    }
    return -1;
}