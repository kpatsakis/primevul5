void bcf_hdr_check_sanity(bcf_hdr_t *hdr)
{
    static int PL_warned = 0, GL_warned = 0;

    if ( !PL_warned )
    {
        int id = bcf_hdr_id2int(hdr, BCF_DT_ID, "PL");
        if ( bcf_hdr_idinfo_exists(hdr,BCF_HL_FMT,id) && bcf_hdr_id2length(hdr,BCF_HL_FMT,id)!=BCF_VL_G )
        {
            hts_log_warning("PL should be declared as Number=G");
            PL_warned = 1;
        }
    }
    if ( !GL_warned )
    {
        int id = bcf_hdr_id2int(hdr, BCF_DT_ID, "GL");
        if ( bcf_hdr_idinfo_exists(hdr,BCF_HL_FMT,id) && bcf_hdr_id2length(hdr,BCF_HL_FMT,id)!=BCF_VL_G )
        {
            hts_log_warning("GL should be declared as Number=G");
            GL_warned = 1;
        }
    }
}