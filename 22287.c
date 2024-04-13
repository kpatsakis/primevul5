static int annotate_dbname_mailbox(struct mailbox *mailbox, char **fnamep)
{
    const char *conf_fname;

    if (!mailbox) return annotate_dbname_mbentry(NULL, fnamep);

    conf_fname = mailbox_meta_fname(mailbox, META_ANNOTATIONS);
    if (!conf_fname) return IMAP_MAILBOX_BADNAME;
    *fnamep = xstrdup(conf_fname);

    return 0;
}