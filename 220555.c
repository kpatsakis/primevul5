vim_mkdir_emsg(char_u *name, int prot UNUSED)
{
    if (vim_mkdir(name, prot) != 0)
    {
	semsg(_("E739: Cannot create directory: %s"), name);
	return FAIL;
    }
    return OK;
}