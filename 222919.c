static void set_flag(struct Mailbox *m, AclFlags aclflag, int flag,
                     const char *str, char *flags, size_t flsize)
{
  if (m->rights & aclflag)
    if (flag && imap_has_flag(&imap_mdata_get(m)->flags, str))
      mutt_str_cat(flags, flsize, str);
}