int bcf_hdr_set_version(bcf_hdr_t *hdr, const char *version)
{
    bcf_hrec_t *hrec = bcf_hdr_get_hrec(hdr, BCF_HL_GEN, "fileformat", NULL, NULL);
    if ( !hrec )
    {
        int len;
        kstring_t str = {0,0,0};
        ksprintf(&str,"##fileformat=%s", version);
        hrec = bcf_hdr_parse_line(hdr, str.s, &len);
        free(str.s);
    }
    else
    {
        free(hrec->value);
        hrec->value = strdup(version);
    }
    hdr->dirty = 1;
    return 0; // FIXME: check for errs in this function (return < 0 if so)
}