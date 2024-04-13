au_remove_pat(AutoPat *ap)
{
    vim_free(ap->pat);
    ap->pat = NULL;
    ap->buflocal_nr = -1;
    au_need_clean = TRUE;
}