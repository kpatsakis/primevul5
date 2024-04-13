getchar_common(typval_T *argvars, typval_T *rettv)
{
    varnumber_T		n;
    int			error = FALSE;

    if (in_vim9script() && check_for_opt_bool_arg(argvars, 0) == FAIL)
	return;

#ifdef MESSAGE_QUEUE
    // vpeekc() used to check for messages, but that caused problems, invoking
    // a callback where it was not expected.  Some plugins use getchar(1) in a
    // loop to await a message, therefore make sure we check for messages here.
    parse_queued_messages();
#endif

    // Position the cursor.  Needed after a message that ends in a space.
    windgoto(msg_row, msg_col);

    ++no_mapping;
    ++allow_keys;
    for (;;)
    {
	if (argvars[0].v_type == VAR_UNKNOWN)
	    // getchar(): blocking wait.
	    n = plain_vgetc();
	else if (tv_get_bool_chk(&argvars[0], &error))
	    // getchar(1): only check if char avail
	    n = vpeekc_any();
	else if (error || vpeekc_any() == NUL)
	    // illegal argument or getchar(0) and no char avail: return zero
	    n = 0;
	else
	    // getchar(0) and char avail() != NUL: get a character.
	    // Note that vpeekc_any() returns K_SPECIAL for K_IGNORE.
	    n = safe_vgetc();

	if (n == K_IGNORE || n == K_MOUSEMOVE
		|| n == K_VER_SCROLLBAR || n == K_HOR_SCROLLBAR)
	    continue;
	break;
    }
    --no_mapping;
    --allow_keys;

    set_vim_var_nr(VV_MOUSE_WIN, 0);
    set_vim_var_nr(VV_MOUSE_WINID, 0);
    set_vim_var_nr(VV_MOUSE_LNUM, 0);
    set_vim_var_nr(VV_MOUSE_COL, 0);

    rettv->vval.v_number = n;
    if (IS_SPECIAL(n) || mod_mask != 0)
    {
	char_u		temp[10];   // modifier: 3, mbyte-char: 6, NUL: 1
	int		i = 0;

	// Turn a special key into three bytes, plus modifier.
	if (mod_mask != 0)
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = KS_MODIFIER;
	    temp[i++] = mod_mask;
	}
	if (IS_SPECIAL(n))
	{
	    temp[i++] = K_SPECIAL;
	    temp[i++] = K_SECOND(n);
	    temp[i++] = K_THIRD(n);
	}
	else if (has_mbyte)
	    i += (*mb_char2bytes)(n, temp + i);
	else
	    temp[i++] = n;
	temp[i++] = NUL;
	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = vim_strsave(temp);

	if (is_mouse_key(n))
	{
	    int		row = mouse_row;
	    int		col = mouse_col;
	    win_T	*win;
	    linenr_T	lnum;
	    win_T	*wp;
	    int		winnr = 1;

	    if (row >= 0 && col >= 0)
	    {
		// Find the window at the mouse coordinates and compute the
		// text position.
		win = mouse_find_win(&row, &col, FIND_POPUP);
		if (win == NULL)
		    return;
		(void)mouse_comp_pos(win, &row, &col, &lnum, NULL);
#ifdef FEAT_PROP_POPUP
		if (WIN_IS_POPUP(win))
		    winnr = 0;
		else
#endif
		    for (wp = firstwin; wp != win && wp != NULL;
							       wp = wp->w_next)
			++winnr;
		set_vim_var_nr(VV_MOUSE_WIN, winnr);
		set_vim_var_nr(VV_MOUSE_WINID, win->w_id);
		set_vim_var_nr(VV_MOUSE_LNUM, lnum);
		set_vim_var_nr(VV_MOUSE_COL, col + 1);
	    }
	}
    }
}