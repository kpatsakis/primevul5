int bcf_hdr_set_samples(bcf_hdr_t *hdr, const char *samples, int is_file)
{
    if ( samples && !strcmp("-",samples) ) return 0;            // keep all samples

    int i, narr = bit_array_size(bcf_hdr_nsamples(hdr));
    hdr->keep_samples = (uint8_t*) calloc(narr,1);
    if (!hdr->keep_samples) return -1;

    hdr->nsamples_ori = bcf_hdr_nsamples(hdr);
    if ( !samples )
    {
        // exclude all samples
        khint_t k;
        vdict_t *d = (vdict_t*)hdr->dict[BCF_DT_SAMPLE], *new_dict;
        new_dict = kh_init(vdict);
        if (!new_dict) return -1;

        bcf_hdr_nsamples(hdr) = 0;

        for (k = kh_begin(d); k != kh_end(d); ++k)
            if (kh_exist(d, k)) free((char*)kh_key(d, k));
        kh_destroy(vdict, d);
        hdr->dict[BCF_DT_SAMPLE] = new_dict;
        if (bcf_hdr_sync(hdr) < 0) return -1;

        return 0;
    }

    if ( samples[0]=='^' )
        for (i=0; i<bcf_hdr_nsamples(hdr); i++) bit_array_set(hdr->keep_samples,i);

    int idx, n, ret = 0;
    char **smpls = hts_readlist(samples[0]=='^'?samples+1:samples, is_file, &n);
    if ( !smpls ) return -1;
    for (i=0; i<n; i++)
    {
        idx = bcf_hdr_id2int(hdr,BCF_DT_SAMPLE,smpls[i]);
        if ( idx<0 )
        {
            if ( !ret ) ret = i+1;
            continue;
        }
        assert( idx<bcf_hdr_nsamples(hdr) );
        if (  samples[0]=='^' )
            bit_array_clear(hdr->keep_samples, idx);
        else
            bit_array_set(hdr->keep_samples, idx);
    }
    for (i=0; i<n; i++) free(smpls[i]);
    free(smpls);

    bcf_hdr_nsamples(hdr) = 0;
    for (i=0; i<hdr->nsamples_ori; i++)
        if ( bit_array_test(hdr->keep_samples,i) ) bcf_hdr_nsamples(hdr)++;

    if ( !bcf_hdr_nsamples(hdr) ) { free(hdr->keep_samples); hdr->keep_samples=NULL; }
    else
    {
        // Make new list and dictionary with desired samples
        char **samples = (char**) malloc(sizeof(char*)*bcf_hdr_nsamples(hdr));
        vdict_t *new_dict, *d;
        int k, res;
        if (!samples) return -1;

        new_dict = kh_init(vdict);
        if (!new_dict) {
            free(samples);
            return -1;
        }
        idx = 0;
        for (i=0; i<hdr->nsamples_ori; i++) {
            if ( bit_array_test(hdr->keep_samples,i) ) {
                samples[idx] = hdr->samples[i];
                k = kh_put(vdict, new_dict, hdr->samples[i], &res);
                if (res < 0) {
                    free(samples);
                    kh_destroy(vdict, new_dict);
                    return -1;
                }
                kh_val(new_dict, k) = bcf_idinfo_def;
                kh_val(new_dict, k).id = idx;
                idx++;
            }
        }

        // Delete desired samples from old dictionary, so we don't free them
        d = (vdict_t*)hdr->dict[BCF_DT_SAMPLE];
        for (i=0; i < idx; i++) {
            int k = kh_get(vdict, d, samples[i]);
            if (k < kh_end(d)) kh_del(vdict, d, k);
        }

        // Free everything else
        for (k = kh_begin(d); k != kh_end(d); ++k)
            if (kh_exist(d, k)) free((char*)kh_key(d, k));
        kh_destroy(vdict, d);
        hdr->dict[BCF_DT_SAMPLE] = new_dict;

        free(hdr->samples);
        hdr->samples = samples;

        if (bcf_hdr_sync(hdr) < 0)
            return -1;
    }

    return ret;
}