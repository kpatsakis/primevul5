merge_modifyOtherKeys(int c_arg, int *modifiers)
{
    int c = c_arg;

    if (*modifiers & MOD_MASK_CTRL)
    {
	if ((c >= '`' && c <= 0x7f) || (c >= '@' && c <= '_'))
	    c &= 0x1f;
	else if (c == '6')
	    // CTRL-6 is equivalent to CTRL-^
	    c = 0x1e;
#ifdef FEAT_GUI_GTK
	// These mappings look arbitrary at the first glance, but in fact
	// resemble quite exactly the behaviour of the GTK+ 1.2 GUI on my
	// machine.  The only difference is BS vs. DEL for CTRL-8 (makes
	// more sense and is consistent with usual terminal behaviour).
	else if (c == '2')
	    c = NUL;
	else if (c >= '3' && c <= '7')
	    c = c ^ 0x28;
	else if (c == '8')
	    c = BS;
	else if (c == '?')
	    c = DEL;
#endif
	if (c != c_arg)
	    *modifiers &= ~MOD_MASK_CTRL;
    }
    if ((*modifiers & (MOD_MASK_META | MOD_MASK_ALT))
	    && c >= 0 && c <= 127)
    {
	c += 0x80;
	*modifiers &= ~(MOD_MASK_META|MOD_MASK_ALT);
    }
    return c;
}