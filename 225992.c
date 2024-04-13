static inline int str2output_unit(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"bytes")) return BYTES_OUT;
        if (!strcasecmp(optarg,"bits")) return BITS_OUT;
        if (!strcasecmp(optarg,"packets")) return PACKETS_OUT;
        if (!strcasecmp(optarg,"errors")) return ERRORS_OUT;
    }
    return BYTES_OUT;
}