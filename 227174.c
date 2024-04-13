bcf_hdr_t *bcf_hdr_read(htsFile *hfp)
{
    if (hfp->format.format == vcf)
        return vcf_hdr_read(hfp);
    if (hfp->format.format != bcf) {
        hts_log_error("Input is not detected as bcf or vcf format");
        return NULL;
    }

    assert(hfp->is_bgzf);

    BGZF *fp = hfp->fp.bgzf;
    uint8_t magic[5];
    bcf_hdr_t *h;
    h = bcf_hdr_init("r");
    if (!h) {
        hts_log_error("Failed to allocate bcf header");
        return NULL;
    }
    if (bgzf_read(fp, magic, 5) != 5)
    {
        hts_log_error("Failed to read the header (reading BCF in text mode?)");
        bcf_hdr_destroy(h);
        return NULL;
    }
    if (strncmp((char*)magic, "BCF\2\2", 5) != 0)
    {
        if (!strncmp((char*)magic, "BCF", 3))
            hts_log_error("Invalid BCF2 magic string: only BCFv2.2 is supported");
        else
            hts_log_error("Invalid BCF2 magic string");
        bcf_hdr_destroy(h);
        return NULL;
    }
    uint8_t buf[4];
    size_t hlen;
    char *htxt = NULL;
    if (bgzf_read(fp, buf, 4) != 4) goto fail;
    hlen = buf[0] | (buf[1] << 8) | (buf[2] << 16) | ((size_t) buf[3] << 24);
    if (hlen >= SIZE_MAX) { errno = ENOMEM; goto fail; }
    htxt = (char*)malloc(hlen + 1);
    if (!htxt) goto fail;
    if (bgzf_read(fp, htxt, hlen) != hlen) goto fail;
    htxt[hlen] = '\0'; // Ensure htxt is terminated
    if ( bcf_hdr_parse(h, htxt) < 0 ) goto fail;
    free(htxt);
    return h;
 fail:
    hts_log_error("Failed to read BCF header");
    free(htxt);
    bcf_hdr_destroy(h);
    return NULL;
}