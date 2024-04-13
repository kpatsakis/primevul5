static int annotate_dbname_mbentry(const mbentry_t *mbentry,
                                   char **fnamep)
{
    const char *conf_fname;

    if (mbentry) {
        /* per-mbox database */
        conf_fname = mbentry_metapath(mbentry, META_ANNOTATIONS, /*isnew*/0);
        if (!conf_fname)
            return IMAP_MAILBOX_BADNAME;
        *fnamep = xstrdup(conf_fname);
    }
    else {
        /* global database */
        conf_fname = config_getstring(IMAPOPT_ANNOTATION_DB_PATH);

        if (conf_fname)
            *fnamep = xstrdup(conf_fname);
        else
            *fnamep = strconcat(config_dir, FNAME_GLOBALANNOTATIONS, (char *)NULL);
    }

    return 0;
}