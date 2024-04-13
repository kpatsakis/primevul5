int bcf_hdr_write(htsFile *hfp, bcf_hdr_t *h)
{
    if (!h) {
        errno = EINVAL;
        return -1;
    }
    if ( h->dirty ) {
        if (bcf_hdr_sync(h) < 0) return -1;
    }
    hfp->format.category = variant_data;
    if (hfp->format.format == vcf || hfp->format.format == text_format) {
        hfp->format.format = vcf;
        return vcf_hdr_write(hfp, h);
    }

    if (hfp->format.format == binary_format)
        hfp->format.format = bcf;

    kstring_t htxt = {0,0,0};
    bcf_hdr_format(h, 1, &htxt);
    kputc('\0', &htxt); // include the \0 byte

    BGZF *fp = hfp->fp.bgzf;
    if ( bgzf_write(fp, "BCF\2\2", 5) !=5 ) return -1;
    uint8_t hlen[4];
    u32_to_le(htxt.l, hlen);
    if ( bgzf_write(fp, hlen, 4) !=4 ) return -1;
    if ( bgzf_write(fp, htxt.s, htxt.l) != htxt.l ) return -1;

    free(htxt.s);
    return 0;
}