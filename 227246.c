static int _bcf_hrec_format(const bcf_hrec_t *hrec, int is_bcf, kstring_t *str)
{
    uint32_t e = 0;
    if ( !hrec->value )
    {
        int j, nout = 0;
        e |= ksprintf(str, "##%s=<", hrec->key) < 0;
        for (j=0; j<hrec->nkeys; j++)
        {
            // do not output IDX if output is VCF
            if ( !is_bcf && !strcmp("IDX",hrec->keys[j]) ) continue;
            if ( nout ) e |= kputc(',',str) < 0;
            e |= ksprintf(str,"%s=%s", hrec->keys[j], hrec->vals[j]) < 0;
            nout++;
        }
        e |= ksprintf(str,">\n") < 0;
    }
    else
        e |= ksprintf(str,"##%s=%s\n", hrec->key,hrec->value) < 0;

    return e == 0 ? 0 : -1;
}