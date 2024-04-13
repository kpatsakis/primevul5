void bcf_hdr_remove(bcf_hdr_t *hdr, int type, const char *key)
{
    int i = 0;
    bcf_hrec_t *hrec;
    if ( !key )
    {
        while ( i<hdr->nhrec )
        {
            if ( hdr->hrec[i]->type!=type ) { i++; continue; }
            hrec = hdr->hrec[i];

            if ( type==BCF_HL_FLT || type==BCF_HL_INFO || type==BCF_HL_FMT || type== BCF_HL_CTG )
            {
                int j = bcf_hrec_find_key(hdr->hrec[i], "ID");
                if ( j>=0 )
                {
                    vdict_t *d = type==BCF_HL_CTG ? (vdict_t*)hdr->dict[BCF_DT_CTG] : (vdict_t*)hdr->dict[BCF_DT_ID];
                    khint_t k = kh_get(vdict, d, hdr->hrec[i]->vals[j]);
                    kh_val(d, k).hrec[type==BCF_HL_CTG?0:type] = NULL;
                }
            }

            hdr->dirty = 1;
            hdr->nhrec--;
            if ( i < hdr->nhrec )
                memmove(&hdr->hrec[i],&hdr->hrec[i+1],(hdr->nhrec-i)*sizeof(bcf_hrec_t*));
            bcf_hrec_destroy(hrec);
        }
        return;
    }
    while (1)
    {
        if ( type==BCF_HL_FLT || type==BCF_HL_INFO || type==BCF_HL_FMT || type== BCF_HL_CTG )
        {
            hrec = bcf_hdr_get_hrec(hdr, type, "ID", key, NULL);
            if ( !hrec ) return;

            for (i=0; i<hdr->nhrec; i++)
                if ( hdr->hrec[i]==hrec ) break;
            assert( i<hdr->nhrec );

            vdict_t *d = type==BCF_HL_CTG ? (vdict_t*)hdr->dict[BCF_DT_CTG] : (vdict_t*)hdr->dict[BCF_DT_ID];
            khint_t k = kh_get(vdict, d, key);
            kh_val(d, k).hrec[type==BCF_HL_CTG?0:type] = NULL;
        }
        else
        {
            for (i=0; i<hdr->nhrec; i++)
            {
                if ( hdr->hrec[i]->type!=type ) continue;
                if ( type==BCF_HL_GEN )
                {
                    if ( !strcmp(hdr->hrec[i]->key,key) ) break;
                }
                else
                {
                    // not all structured lines have ID, we could be more sophisticated as in bcf_hdr_get_hrec()
                    int j = bcf_hrec_find_key(hdr->hrec[i], "ID");
                    if ( j>=0 && !strcmp(hdr->hrec[i]->vals[j],key) ) break;
                }
            }
            if ( i==hdr->nhrec ) return;
            hrec = hdr->hrec[i];
        }

        hdr->nhrec--;
        if ( i < hdr->nhrec )
            memmove(&hdr->hrec[i],&hdr->hrec[i+1],(hdr->nhrec-i)*sizeof(bcf_hrec_t*));
        bcf_hrec_destroy(hrec);
        hdr->dirty = 1;
    }
}