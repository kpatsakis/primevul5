static int identity_concat(void *v, const char *key, const char *val)
{
    char *slider = v;
    int length = strlen(slider);
    slider += length;
    if (length) {
        *slider = '&';
        slider++;
    }
    ap_escape_urlencoded_buffer(slider, key);
    slider += strlen(slider);
    *slider = '=';
    slider++;
    ap_escape_urlencoded_buffer(slider, val);
    return 1;
}