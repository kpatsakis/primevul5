spell_message(spellinfo_T *spin, char_u *str)
{
    if (spin->si_verbose || p_verbose > 2)
    {
	if (!spin->si_verbose)
	    verbose_enter();
	MSG(str);
	out_flush();
	if (!spin->si_verbose)
	    verbose_leave();
    }
}