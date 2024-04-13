int bcf_hrec_set_val(bcf_hrec_t *hrec, int i, const char *str, size_t len, int is_quoted)
{
    if ( hrec->vals[i] ) {
        free(hrec->vals[i]);
        hrec->vals[i] = NULL;
    }
    if ( !str ) return 0;
    if ( is_quoted )
    {
        if (len >= SIZE_MAX - 3) {
            errno = ENOMEM;
            return -1;
        }
        hrec->vals[i] = (char*) malloc((len+3)*sizeof(char));
        if (!hrec->vals[i]) return -1;
        hrec->vals[i][0] = '"';
        memcpy(&hrec->vals[i][1],str,len);
        hrec->vals[i][len+1] = '"';
        hrec->vals[i][len+2] = 0;
    }
    else
    {
        if (len == SIZE_MAX) {
            errno = ENOMEM;
            return -1;
        }
        hrec->vals[i] = (char*) malloc((len+1)*sizeof(char));
        if (!hrec->vals[i]) return -1;
        memcpy(hrec->vals[i],str,len);
        hrec->vals[i][len] = 0;
    }
    return 0;
}