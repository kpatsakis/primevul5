int vcf_write_line(htsFile *fp, kstring_t *line)
{
    int ret;
    if ( line->s[line->l-1]!='\n' ) kputc('\n',line);
    if ( fp->format.compression!=no_compression )
        ret = bgzf_write(fp->fp.bgzf, line->s, line->l);
    else
        ret = hwrite(fp->fp.hfile, line->s, line->l);
    return ret==line->l ? 0 : -1;
}