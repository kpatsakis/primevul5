int vcf_hdr_write(htsFile *fp, const bcf_hdr_t *h)
{
    kstring_t htxt = {0,0,0};
    bcf_hdr_format(h, 0, &htxt);
    while (htxt.l && htxt.s[htxt.l-1] == '\0') --htxt.l; // kill trailing zeros
    int ret;
    if ( fp->format.compression!=no_compression )
        ret = bgzf_write(fp->fp.bgzf, htxt.s, htxt.l);
    else
        ret = hwrite(fp->fp.hfile, htxt.s, htxt.l);
    free(htxt.s);
    return ret<0 ? -1 : 0;
}