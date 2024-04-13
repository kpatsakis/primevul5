bcf_hrec_t *bcf_hdr_get_hrec(const bcf_hdr_t *hdr, int type, const char *key, const char *value, const char *str_class)
{
    int i;
    if ( type==BCF_HL_GEN )
    {
        for (i=0; i<hdr->nhrec; i++)
        {
            if ( hdr->hrec[i]->type!=type ) continue;
            if ( strcmp(hdr->hrec[i]->key,key) ) continue;
            if ( !value || !strcmp(hdr->hrec[i]->value,value) ) return hdr->hrec[i];
        }
        return NULL;
    }
    else if ( type==BCF_HL_STR )
    {
        for (i=0; i<hdr->nhrec; i++)
        {
            if ( hdr->hrec[i]->type!=type ) continue;
            if ( strcmp(hdr->hrec[i]->key,str_class) ) continue;
            int j = bcf_hrec_find_key(hdr->hrec[i],key);
            if ( j>=0 && !strcmp(hdr->hrec[i]->vals[j],value) ) return hdr->hrec[i];
        }
        return NULL;
    }
    vdict_t *d = type==BCF_HL_CTG ? (vdict_t*)hdr->dict[BCF_DT_CTG] : (vdict_t*)hdr->dict[BCF_DT_ID];
    khint_t k = kh_get(vdict, d, value);
    if ( k == kh_end(d) ) return NULL;
    return kh_val(d, k).hrec[type==BCF_HL_CTG?0:type];
}