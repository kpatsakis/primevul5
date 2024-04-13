bcf_hdr_t *vcf_hdr_read(htsFile *fp)
{
    kstring_t txt, *s = &fp->line;
    int ret;
    bcf_hdr_t *h;
    tbx_t *idx = NULL;
    const char **names = NULL;
    h = bcf_hdr_init("r");
    if (!h) {
        hts_log_error("Failed to allocate bcf header");
        return NULL;
    }
    txt.l = txt.m = 0; txt.s = 0;
    while ((ret = hts_getline(fp, KS_SEP_LINE, s)) >= 0) {
        int e = 0;
        if (s->l == 0) continue;
        if (s->s[0] != '#') {
            hts_log_error("No sample line");
            goto error;
        }
        if (s->s[1] != '#' && fp->fn_aux) { // insert contigs here
            kstring_t tmp = { 0, 0, NULL };
            hFILE *f = hopen(fp->fn_aux, "r");
            if (f == NULL) {
                hts_log_error("Couldn't open \"%s\"", fp->fn_aux);
                goto error;
            }
            while (tmp.l = 0, kgetline(&tmp, (kgets_func *) hgets, f) >= 0) {
                char *tab = strchr(tmp.s, '\t');
                if (tab == NULL) continue;
                e |= (kputs("##contig=<ID=", &txt) < 0);
                e |= (kputsn(tmp.s, tab - tmp.s, &txt) < 0);
                e |= (kputs(",length=", &txt) < 0);
                e |= (kputl(atol(tab), &txt) < 0);
                e |= (kputsn(">\n", 2, &txt) < 0);
            }
            free(tmp.s);
            if (hclose(f) != 0) {
                hts_log_error("Error on closing %s", fp->fn_aux);
                goto error;
            }
            if (e) goto error;
        }
        if (kputsn(s->s, s->l, &txt) < 0) goto error;
        if (kputc('\n', &txt) < 0) goto error;
        if (s->s[1] != '#') break;
    }
    if ( ret < -1 ) goto error;
    if ( !txt.s )
    {
        hts_log_error("Could not read the header");
        goto error;
    }
    if ( bcf_hdr_parse(h, txt.s) < 0 ) goto error;

    // check tabix index, are all contigs listed in the header? add the missing ones
    idx = tbx_index_load3(fp->fn, NULL, HTS_IDX_SAVE_REMOTE|HTS_IDX_SILENT_FAIL);
    if ( idx )
    {
        int i, n, need_sync = 0;
        names = tbx_seqnames(idx, &n);
        if (!names) goto error;
        for (i=0; i<n; i++)
        {
            bcf_hrec_t *hrec = bcf_hdr_get_hrec(h, BCF_HL_CTG, "ID", (char*) names[i], NULL);
            if ( hrec ) continue;
            if (add_missing_contig_hrec(h, names[i]) < 0) goto error;
            need_sync = 1;
        }
        if ( need_sync ) {
            if (bcf_hdr_sync(h) < 0) goto error;
        }
        free(names);
        tbx_destroy(idx);
    }
    free(txt.s);
    return h;

 error:
    if (idx) tbx_destroy(idx);
    free(names);
    free(txt.s);
    if (h) bcf_hdr_destroy(h);
    return NULL;
}