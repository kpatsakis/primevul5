int vcf_open_mode(char *mode, const char *fn, const char *format)
{
    if (format == NULL) {
        // Try to pick a format based on the filename extension
        char extension[HTS_MAX_EXT_LEN];
        if (find_file_extension(fn, extension) < 0) return -1;
        return vcf_open_mode(mode, fn, extension);
    }
    else if (strcasecmp(format, "bcf") == 0) strcpy(mode, "b");
    else if (strcasecmp(format, "vcf") == 0) strcpy(mode, "");
    else if (strcasecmp(format, "vcf.gz") == 0 || strcasecmp(format, "vcf.bgz") == 0) strcpy(mode, "z");
    else return -1;

    return 0;
}