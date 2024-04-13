ins_char_typebuf(int c, int modifier)
{
    char_u	buf[MB_MAXBYTES * 3 + 4];
    int		len = 0;

    if (modifier != 0)
    {
	buf[0] = K_SPECIAL;
	buf[1] = KS_MODIFIER;
	buf[2] = modifier;
	buf[3] = NUL;
	len = 3;
    }
    if (IS_SPECIAL(c))
    {
	buf[len] = K_SPECIAL;
	buf[len + 1] = K_SECOND(c);
	buf[len + 2] = K_THIRD(c);
	buf[len + 3] = NUL;
	len += 3;
    }
    else
    {
	char_u	*p = buf + len;
	int	char_len = (*mb_char2bytes)(c, p);
#ifdef FEAT_GUI
	int	save_gui_in_use = gui.in_use;

	gui.in_use = FALSE;
#endif
	// if the character contains CSI or K_SPECIAL bytes they need escaping
	len += fix_input_buffer(p, char_len);
#ifdef FEAT_GUI
	gui.in_use = save_gui_in_use;
#endif
    }
    (void)ins_typebuf(buf, KeyNoremap, 0, !KeyTyped, cmd_silent);
    return len;
}