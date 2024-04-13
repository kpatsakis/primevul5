static int bcf_hdr_set_idx(bcf_hdr_t *hdr, int dict_type, const char *tag, bcf_idinfo_t *idinfo)
{
    size_t new_n;

    // If available, preserve existing IDX
    if ( idinfo->id==-1 )
        idinfo->id = hdr->n[dict_type];
    else if ( idinfo->id < hdr->n[dict_type] && hdr->id[dict_type][idinfo->id].key )
    {
        hts_log_error("Conflicting IDX=%d lines in the header dictionary, the new tag is %s",
            idinfo->id, tag);
        errno = EINVAL;
        return -1;
    }

    new_n = idinfo->id >= hdr->n[dict_type] ? idinfo->id+1 : hdr->n[dict_type];
    if (hts_resize(bcf_idpair_t, new_n, &hdr->m[dict_type],
                   &hdr->id[dict_type], HTS_RESIZE_CLEAR)) {
        return -1;
    }
    hdr->n[dict_type] = new_n;

    // NB: the next kh_put call can invalidate the idinfo pointer, therefore
    // we leave it unassigned here. It must be set explicitly in bcf_hdr_sync.
    hdr->id[dict_type][idinfo->id].key = tag;

    return 0;
}