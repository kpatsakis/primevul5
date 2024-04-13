static inline int str2output_type(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"rate")) return RATE_OUT;
        if (!strcasecmp(optarg,"max")) return MAX_OUT;
        if (!strcasecmp(optarg,"sum")) return SUM_OUT;
        if (!strcasecmp(optarg,"avg")) return AVG_OUT;
    }
    return RATE_OUT;
}