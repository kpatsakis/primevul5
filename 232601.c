au_remove_cmds(AutoPat *ap)
{
    AutoCmd *ac;

    for (ac = ap->cmds; ac != NULL; ac = ac->next)
    {
	vim_free(ac->cmd);
	ac->cmd = NULL;
    }
    au_need_clean = TRUE;
}