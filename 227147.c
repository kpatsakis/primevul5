static int bcf_hdr_register_hrec(bcf_hdr_t *hdr, bcf_hrec_t *hrec)
{
    // contig
    int i, ret, replacing = 0;
    khint_t k;
    char *str = NULL;

    if ( !strcmp(hrec->key, "contig") )
    {
        hts_pos_t len = 0;
        hrec->type = BCF_HL_CTG;

        // Get the contig ID ($str) and length ($j)
        i = bcf_hrec_find_key(hrec,"length");
        if ( i<0 ) len = 0;
        else {
            char *end = hrec->vals[i];
            len = strtoll(hrec->vals[i], &end, 10);
            if (end == hrec->vals[i] || len < 0) return 0;
        }

        i = bcf_hrec_find_key(hrec,"ID");
        if ( i<0 ) return 0;
        str = strdup(hrec->vals[i]);
        if (!str) return -1;

        // Register in the dictionary
        vdict_t *d = (vdict_t*)hdr->dict[BCF_DT_CTG];
        khint_t k = kh_get(vdict, d, str);
        if ( k != kh_end(d) ) { // already present
            free(str); str=NULL;
            if (kh_val(d, k).hrec[0] != NULL) // and not removed
                return 0;
            replacing = 1;
        } else {
            k = kh_put(vdict, d, str, &ret);
            if (ret < 0) { free(str); return -1; }
        }

        int idx = bcf_hrec_find_key(hrec,"IDX");
        if ( idx!=-1 )
        {
            char *tmp = hrec->vals[idx];
            idx = strtol(hrec->vals[idx], &tmp, 10);
            if ( *tmp || idx < 0 || idx >= INT_MAX - 1)
            {
                if (!replacing) {
                    kh_del(vdict, d, k);
                    free(str);
                }
                hts_log_warning("Error parsing the IDX tag, skipping");
                return 0;
            }
        }

        kh_val(d, k) = bcf_idinfo_def;
        kh_val(d, k).id = idx;
        kh_val(d, k).info[0] = len;
        kh_val(d, k).hrec[0] = hrec;
        if (bcf_hdr_set_idx(hdr, BCF_DT_CTG, kh_key(d,k), &kh_val(d,k)) < 0) {
            if (!replacing) {
                kh_del(vdict, d, k);
                free(str);
            }
            return -1;
        }
        if ( idx==-1 ) {
            if (hrec_add_idx(hrec, kh_val(d,k).id) < 0) {
               return -1;
            }
        }

        return 1;
    }

    if ( !strcmp(hrec->key, "INFO") ) hrec->type = BCF_HL_INFO;
    else if ( !strcmp(hrec->key, "FILTER") ) hrec->type = BCF_HL_FLT;
    else if ( !strcmp(hrec->key, "FORMAT") ) hrec->type = BCF_HL_FMT;
    else if ( hrec->nkeys>0 ) { hrec->type = BCF_HL_STR; return 1; }
    else return 0;

    // INFO/FILTER/FORMAT
    char *id = NULL;
    uint32_t type = UINT32_MAX, var = UINT32_MAX;
    int num = -1, idx = -1;
    for (i=0; i<hrec->nkeys; i++)
    {
        if ( !strcmp(hrec->keys[i], "ID") ) id = hrec->vals[i];
        else if ( !strcmp(hrec->keys[i], "IDX") )
        {
            char *tmp = hrec->vals[i];
            idx = strtol(hrec->vals[i], &tmp, 10);
            if ( *tmp || idx < 0 || idx >= INT_MAX - 1)
            {
                hts_log_warning("Error parsing the IDX tag, skipping");
                return 0;
            }
        }
        else if ( !strcmp(hrec->keys[i], "Type") )
        {
            if ( !strcmp(hrec->vals[i], "Integer") ) type = BCF_HT_INT;
            else if ( !strcmp(hrec->vals[i], "Float") ) type = BCF_HT_REAL;
            else if ( !strcmp(hrec->vals[i], "String") ) type = BCF_HT_STR;
            else if ( !strcmp(hrec->vals[i], "Character") ) type = BCF_HT_STR;
            else if ( !strcmp(hrec->vals[i], "Flag") ) type = BCF_HT_FLAG;
            else
            {
                hts_log_warning("The type \"%s\" is not supported, assuming \"String\"", hrec->vals[i]);
                type = BCF_HT_STR;
            }
        }
        else if ( !strcmp(hrec->keys[i], "Number") )
        {
            if ( !strcmp(hrec->vals[i],"A") ) var = BCF_VL_A;
            else if ( !strcmp(hrec->vals[i],"R") ) var = BCF_VL_R;
            else if ( !strcmp(hrec->vals[i],"G") ) var = BCF_VL_G;
            else if ( !strcmp(hrec->vals[i],".") ) var = BCF_VL_VAR;
            else
            {
                sscanf(hrec->vals[i],"%d",&num);
                var = BCF_VL_FIXED;
            }
            if (var != BCF_VL_FIXED) num = 0xfffff;
        }
    }
    if (hrec->type == BCF_HL_INFO || hrec->type == BCF_HL_FMT) {
        if (type == -1) {
            hts_log_warning("%s %s field has no Type defined. Assuming String",
                *hrec->key == 'I' ? "An" : "A", hrec->key);
            type = BCF_HT_STR;
        }
        if (var == -1) {
            hts_log_warning("%s %s field has no Number defined. Assuming '.'",
                *hrec->key == 'I' ? "An" : "A", hrec->key);
            var = BCF_VL_VAR;
        }
    }
    uint32_t info = ((((uint32_t)num) & 0xfffff)<<12 |
                     (var & 0xf) << 8 |
                     (type & 0xf) << 4 |
                     (((uint32_t) hrec->type) & 0xf));

    if ( !id ) return 0;
    str = strdup(id);
    if (!str) return -1;

    vdict_t *d = (vdict_t*)hdr->dict[BCF_DT_ID];
    k = kh_get(vdict, d, str);
    if ( k != kh_end(d) )
    {
        // already present
        free(str);
        if ( kh_val(d, k).hrec[info&0xf] ) return 0;
        kh_val(d, k).info[info&0xf] = info;
        kh_val(d, k).hrec[info&0xf] = hrec;
        if ( idx==-1 ) {
            if (hrec_add_idx(hrec, kh_val(d, k).id) < 0) {
                return -1;
            }
        }
        return 1;
    }
    k = kh_put(vdict, d, str, &ret);
    if (ret < 0) {
        free(str);
        return -1;
    }
    kh_val(d, k) = bcf_idinfo_def;
    kh_val(d, k).info[info&0xf] = info;
    kh_val(d, k).hrec[info&0xf] = hrec;
    kh_val(d, k).id = idx;
    if (bcf_hdr_set_idx(hdr, BCF_DT_ID, kh_key(d,k), &kh_val(d,k)) < 0) {
        kh_del(vdict, d, k);
        free(str);
        return -1;
    }
    if ( idx==-1 ) {
        if (hrec_add_idx(hrec, kh_val(d,k).id) < 0) {
            return -1;
        }
    }

    return 1;
}