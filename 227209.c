int bcf_hdr_combine(bcf_hdr_t *dst, const bcf_hdr_t *src)
{
    int i, ndst_ori = dst->nhrec, need_sync = 0, ret = 0, res;
    for (i=0; i<src->nhrec; i++)
    {
        if ( src->hrec[i]->type==BCF_HL_GEN && src->hrec[i]->value )
        {
            int j;
            for (j=0; j<ndst_ori; j++)
            {
                if ( dst->hrec[j]->type!=BCF_HL_GEN ) continue;

                // Checking only the key part of generic lines, otherwise
                // the VCFs are too verbose. Should we perhaps add a flag
                // to bcf_hdr_combine() and make this optional?
                if ( !strcmp(src->hrec[i]->key,dst->hrec[j]->key) ) break;
            }
            if ( j>=ndst_ori ) {
                res = bcf_hdr_add_hrec(dst, bcf_hrec_dup(src->hrec[i]));
                if (res < 0) return -1;
                need_sync += res;
            }
        }
        else if ( src->hrec[i]->type==BCF_HL_STR )
        {
            // NB: we are ignoring fields without ID
            int j = bcf_hrec_find_key(src->hrec[i],"ID");
            if ( j>=0 )
            {
                bcf_hrec_t *rec = bcf_hdr_get_hrec(dst, src->hrec[i]->type, "ID", src->hrec[i]->vals[j], src->hrec[i]->key);
                if ( !rec ) {
                    res = bcf_hdr_add_hrec(dst, bcf_hrec_dup(src->hrec[i]));
                    if (res < 0) return -1;
                    need_sync += res;
                }
            }
        }
        else
        {
            int j = bcf_hrec_find_key(src->hrec[i],"ID");
            assert( j>=0 ); // this should always be true for valid VCFs

            bcf_hrec_t *rec = bcf_hdr_get_hrec(dst, src->hrec[i]->type, "ID", src->hrec[i]->vals[j], NULL);
            if ( !rec ) {
                res = bcf_hdr_add_hrec(dst, bcf_hrec_dup(src->hrec[i]));
                if (res < 0) return -1;
                need_sync += res;
            } else if ( src->hrec[i]->type==BCF_HL_INFO || src->hrec[i]->type==BCF_HL_FMT )
            {
                // Check that both records are of the same type. The bcf_hdr_id2length
                // macro cannot be used here because dst header is not synced yet.
                vdict_t *d_src = (vdict_t*)src->dict[BCF_DT_ID];
                vdict_t *d_dst = (vdict_t*)dst->dict[BCF_DT_ID];
                khint_t k_src  = kh_get(vdict, d_src, src->hrec[i]->vals[0]);
                khint_t k_dst  = kh_get(vdict, d_dst, src->hrec[i]->vals[0]);
                if ( (kh_val(d_src,k_src).info[rec->type]>>8 & 0xf) != (kh_val(d_dst,k_dst).info[rec->type]>>8 & 0xf) )
                {
                    hts_log_warning("Trying to combine \"%s\" tag definitions of different lengths",
                        src->hrec[i]->vals[0]);
                    ret |= 1;
                }
                if ( (kh_val(d_src,k_src).info[rec->type]>>4 & 0xf) != (kh_val(d_dst,k_dst).info[rec->type]>>4 & 0xf) )
                {
                    hts_log_warning("Trying to combine \"%s\" tag definitions of different types",
                        src->hrec[i]->vals[0]);
                    ret |= 1;
                }
            }
        }
    }
    if ( need_sync ) {
        if (bcf_hdr_sync(dst) < 0) return -1;
    }
    return ret;
}