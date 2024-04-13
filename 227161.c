int bcf_update_format_string(const bcf_hdr_t *hdr, bcf1_t *line, const char *key, const char **values, int n)
{
    if ( !n )
        return bcf_update_format(hdr,line,key,NULL,0,BCF_HT_STR);

    int i, max_len = 0;
    for (i=0; i<n; i++)
    {
        int len = strlen(values[i]);
        if ( len > max_len ) max_len = len;
    }
    char *out = (char*) malloc(max_len*n);
    if ( !out ) return -2;
    for (i=0; i<n; i++)
    {
        char *dst = out+i*max_len;
        const char *src = values[i];
        int j = 0;
        while ( src[j] ) { dst[j] = src[j]; j++; }
        for (; j<max_len; j++) dst[j] = 0;
    }
    int ret = bcf_update_format(hdr,line,key,out,max_len*n,BCF_HT_STR);
    free(out);
    return ret;
}