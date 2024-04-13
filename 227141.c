int bcf_hdr_id2int(const bcf_hdr_t *h, int which, const char *id)
{
    khint_t k;
    vdict_t *d = (vdict_t*)h->dict[which];
    k = kh_get(vdict, d, id);
    return k == kh_end(d)? -1 : kh_val(d, k).id;
}