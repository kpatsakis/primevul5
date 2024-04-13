static char *find_chrom_header_line(char *s)
{
    char *nl;
    if (strncmp(s, "#CHROM\t", 7) == 0) return s;
    else if ((nl = strstr(s, "\n#CHROM\t")) != NULL) return nl+1;
    else return NULL;
}