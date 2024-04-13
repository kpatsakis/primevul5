int bcf_hrec_format(const bcf_hrec_t *hrec, kstring_t *str)
{
    return _bcf_hrec_format(hrec,0,str);
}