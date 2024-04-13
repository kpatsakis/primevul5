static int bcf_record_check(const bcf_hdr_t *hdr, bcf1_t *rec) {
    uint8_t *ptr, *end;
    size_t bytes;
    uint32_t err = 0;
    int type = 0;
    int num  = 0;
    int reflen = 0;
    uint32_t i, reports;
    const uint32_t is_integer = ((1 << BCF_BT_INT8)  |
                                 (1 << BCF_BT_INT16) |
#ifdef VCF_ALLOW_INT64
                                 (1 << BCF_BT_INT64) |
#endif
                                 (1 << BCF_BT_INT32));
    const uint32_t is_valid_type = (is_integer          |
                                    (1 << BCF_BT_NULL)  |
                                    (1 << BCF_BT_FLOAT) |
                                    (1 << BCF_BT_CHAR));
    int32_t max_id = hdr ? hdr->n[BCF_DT_ID] : 0;

    // Check for valid contig ID
    if (rec->rid < 0
        || (hdr && (rec->rid >= hdr->n[BCF_DT_CTG]
                    || hdr->id[BCF_DT_CTG][rec->rid].key == NULL))) {
        hts_log_warning("Bad BCF record at %"PRIhts_pos": Invalid %s id %d", rec->pos+1, "CONTIG", rec->rid);
        err |= BCF_ERR_CTG_INVALID;
    }

    // Check ID
    ptr = (uint8_t *) rec->shared.s;
    end = ptr + rec->shared.l;
    if (bcf_dec_size_safe(ptr, end, &ptr, &num, &type) != 0) goto bad_shared;
    if (type != BCF_BT_CHAR) {
        hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s type %d (%s)", bcf_seqname_safe(hdr,rec), rec->pos+1, "ID", type, get_type_name(type));
        err |= BCF_ERR_TAG_INVALID;
    }
    bytes = (size_t) num << bcf_type_shift[type];
    if (end - ptr < bytes) goto bad_shared;
    ptr += bytes;

    // Check REF and ALT
    reports = 0;
    for (i = 0; i < rec->n_allele; i++) {
        if (bcf_dec_size_safe(ptr, end, &ptr, &num, &type) != 0) goto bad_shared;
        if (type != BCF_BT_CHAR) {
            if (!reports++ || hts_verbose >= HTS_LOG_DEBUG)
                hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s type %d (%s)", bcf_seqname_safe(hdr,rec), rec->pos+1, "REF/ALT", type, get_type_name(type));
            err |= BCF_ERR_CHAR;
        }
        if (i == 0) reflen = num;
        bytes = (size_t) num << bcf_type_shift[type];
        if (end - ptr < bytes) goto bad_shared;
        ptr += bytes;
    }

    // Check FILTER
    reports = 0;
    if (bcf_dec_size_safe(ptr, end, &ptr, &num, &type) != 0) goto bad_shared;
    if (num > 0) {
        bytes = (size_t) num << bcf_type_shift[type];
        if (((1 << type) & is_integer) == 0) {
            hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s type %d (%s)", bcf_seqname_safe(hdr,rec), rec->pos+1, "FILTER", type, get_type_name(type));
            err |= BCF_ERR_TAG_INVALID;
            if (end - ptr < bytes) goto bad_shared;
            ptr += bytes;
        } else {
            if (end - ptr < bytes) goto bad_shared;
            for (i = 0; i < num; i++) {
                int32_t key = bcf_dec_int1(ptr, type, &ptr);
                if (key < 0
                    || (hdr && (key >= max_id
                                || hdr->id[BCF_DT_ID][key].key == NULL))) {
                    if (!reports++ || hts_verbose >= HTS_LOG_DEBUG)
                        hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s id %d", bcf_seqname_safe(hdr,rec), rec->pos+1, "FILTER", key);
                    err |= BCF_ERR_TAG_UNDEF;
                }
            }
        }
    }

    // Check INFO
    reports = 0;
    for (i = 0; i < rec->n_info; i++) {
        int32_t key = -1;
        if (bcf_dec_typed_int1_safe(ptr, end, &ptr, &key) != 0) goto bad_shared;
        if (key < 0 || (hdr && (key >= max_id
                                || hdr->id[BCF_DT_ID][key].key == NULL))) {
            if (!reports++ || hts_verbose >= HTS_LOG_DEBUG)
                hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s id %d", bcf_seqname_safe(hdr,rec), rec->pos+1, "INFO", key);
            err |= BCF_ERR_TAG_UNDEF;
        }
        if (bcf_dec_size_safe(ptr, end, &ptr, &num, &type) != 0) goto bad_shared;
        if (((1 << type) & is_valid_type) == 0) {
            if (!reports++ || hts_verbose >= HTS_LOG_DEBUG)
                hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s type %d (%s)", bcf_seqname_safe(hdr,rec), rec->pos+1, "INFO", type, get_type_name(type));
            err |= BCF_ERR_TAG_INVALID;
        }
        bytes = (size_t) num << bcf_type_shift[type];
        if (end - ptr < bytes) goto bad_shared;
        ptr += bytes;
    }

    // Check FORMAT and individual information
    ptr = (uint8_t *) rec->indiv.s;
    end = ptr + rec->indiv.l;
    reports = 0;
    for (i = 0; i < rec->n_fmt; i++) {
        int32_t key = -1;
        if (bcf_dec_typed_int1_safe(ptr, end, &ptr, &key) != 0) goto bad_indiv;
        if (key < 0
            || (hdr && (key >= max_id
                        || hdr->id[BCF_DT_ID][key].key == NULL))) {
            if (!reports++ || hts_verbose >= HTS_LOG_DEBUG)
                hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s id %d", bcf_seqname_safe(hdr,rec), rec->pos+1, "FORMAT", key);
            err |= BCF_ERR_TAG_UNDEF;
        }
        if (bcf_dec_size_safe(ptr, end, &ptr, &num, &type) != 0) goto bad_indiv;
        if (((1 << type) & is_valid_type) == 0) {
            if (!reports++ || hts_verbose >= HTS_LOG_DEBUG)
                hts_log_warning("Bad BCF record at %s:%"PRIhts_pos": Invalid %s type %d (%s)", bcf_seqname_safe(hdr,rec), rec->pos+1, "FORMAT", type, get_type_name(type));
            err |= BCF_ERR_TAG_INVALID;
        }
        bytes = ((size_t) num << bcf_type_shift[type]) * rec->n_sample;
        if (end - ptr < bytes) goto bad_indiv;
        ptr += bytes;
    }

    if (!err && rec->rlen < 0) {
        // Treat bad rlen as a warning instead of an error, and try to
        // fix up by using the length of the stored REF allele.
        static int warned = 0;
        if (!warned) {
            hts_log_warning("BCF record at %s:%"PRIhts_pos" has invalid RLEN (%"PRIhts_pos"). "
                            "Only one invalid RLEN will be reported.",
                            bcf_seqname_safe(hdr,rec), rec->pos+1, rec->rlen);
            warned = 1;
        }
        rec->rlen = reflen >= 0 ? reflen : 0;
    }

    rec->errcode |= err;

    return err ? -2 : 0; // Return -2 so bcf_read() reports an error

 bad_shared:
    hts_log_error("Bad BCF record at %s:%"PRIhts_pos" - shared section malformed or too short", bcf_seqname_safe(hdr,rec), rec->pos+1);
    return -2;

 bad_indiv:
    hts_log_error("Bad BCF record at %s:%"PRIhts_pos" - individuals section malformed or too short", bcf_seqname_safe(hdr,rec), rec->pos+1);
    return -2;
}