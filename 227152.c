bcf_fmt_t *bcf_get_fmt(const bcf_hdr_t *hdr, bcf1_t *line, const char *key)
{
    int id = bcf_hdr_id2int(hdr, BCF_DT_ID, key);
    if ( !bcf_hdr_idinfo_exists(hdr,BCF_HL_FMT,id) ) return NULL;   // no such FMT field in the header
    return bcf_get_fmt_id(line, id);
}