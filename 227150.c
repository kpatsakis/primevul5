int bcf_hdr_append(bcf_hdr_t *hdr, const char *line)
{
    int len;
    bcf_hrec_t *hrec = bcf_hdr_parse_line(hdr, (char*) line, &len);
    if ( !hrec ) return -1;
    if (bcf_hdr_add_hrec(hdr, hrec) < 0)
        return -1;
    return 0;
}