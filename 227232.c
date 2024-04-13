int bcf_has_filter(const bcf_hdr_t *hdr, bcf1_t *line, char *filter)
{
    if ( filter[0]=='.' && !filter[1] ) filter = "PASS";
    int id = bcf_hdr_id2int(hdr, BCF_DT_ID, filter);
    if ( !bcf_hdr_idinfo_exists(hdr,BCF_HL_FLT,id) ) return -1;  // not defined in the header

    if ( !(line->unpacked & BCF_UN_FLT) ) bcf_unpack(line, BCF_UN_FLT);
    if ( id==0 && !line->d.n_flt) return 1; // PASS

    int i;
    for (i=0; i<line->d.n_flt; i++)
        if ( line->d.flt[i]==id ) return 1;
    return 0;
}