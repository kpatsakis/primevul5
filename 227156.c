int bcf_hdr_format(const bcf_hdr_t *hdr, int is_bcf, kstring_t *str)
{
    int i;
    for (i=0; i<hdr->nhrec; i++)
        _bcf_hrec_format(hdr->hrec[i], is_bcf, str);

    ksprintf(str, "#CHROM\tPOS\tID\tREF\tALT\tQUAL\tFILTER\tINFO");
    if ( bcf_hdr_nsamples(hdr) )
    {
        ksprintf(str, "\tFORMAT");
        for (i=0; i<bcf_hdr_nsamples(hdr); i++)
            ksprintf(str, "\t%s", hdr->samples[i]);
    }
    ksprintf(str, "\n");

    return 0;
}