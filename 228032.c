handle_mapping(
	    int *keylenp,
	    int *timedout,
	    int *mapdepth)
{
    mapblock_T	*mp = NULL;
    mapblock_T	*mp2;
    mapblock_T	*mp_match;
    int		mp_match_len = 0;
    int		max_mlen = 0;
    int		tb_c1;
    int		mlen;
#ifdef FEAT_LANGMAP
    int		nolmaplen;
#endif
    int		keylen = *keylenp;
    int		i;
    int		local_State = get_real_state();

    /*
     * Check for a mappable key sequence.
     * Walk through one maphash[] list until we find an entry that matches.
     *
     * Don't look for mappings if:
     * - no_mapping set: mapping disabled (e.g. for CTRL-V)
     * - maphash_valid not set: no mappings present.
     * - typebuf.tb_buf[typebuf.tb_off] should not be remapped
     * - in insert or cmdline mode and 'paste' option set
     * - waiting for "hit return to continue" and CR or SPACE typed
     * - waiting for a char with --more--
     * - in Ctrl-X mode, and we get a valid char for that mode
     */
    tb_c1 = typebuf.tb_buf[typebuf.tb_off];
    if (no_mapping == 0 && is_maphash_valid()
	    && (no_zero_mapping == 0 || tb_c1 != '0')
	    && (typebuf.tb_maplen == 0
		|| (p_remap
		    && (typebuf.tb_noremap[typebuf.tb_off]
				    & (RM_NONE|RM_ABBR)) == 0))
	    && !(p_paste && (State & (INSERT + CMDLINE)))
	    && !(State == HITRETURN && (tb_c1 == CAR || tb_c1 == ' '))
	    && State != ASKMORE
	    && State != CONFIRM
	    && !((ctrl_x_mode_not_default() && at_ctrl_x_key())
		    || (compl_status_local()
			&& (tb_c1 == Ctrl_N || tb_c1 == Ctrl_P))))
    {
#ifdef FEAT_GUI
	if (gui.in_use && tb_c1 == CSI && typebuf.tb_len >= 2
		&& typebuf.tb_buf[typebuf.tb_off + 1] == KS_MODIFIER)
	{
	    // The GUI code sends CSI KS_MODIFIER {flags}, but mappings expect
	    // K_SPECIAL KS_MODIFIER {flags}.
	    tb_c1 = K_SPECIAL;
	}
#endif
#ifdef FEAT_LANGMAP
	if (tb_c1 == K_SPECIAL)
	    nolmaplen = 2;
	else
	{
	    LANGMAP_ADJUST(tb_c1, (State & (CMDLINE | INSERT)) == 0
					    && get_real_state() != SELECTMODE);
	    nolmaplen = 0;
	}
#endif
	// First try buffer-local mappings.
	mp = get_buf_maphash_list(local_State, tb_c1);
	mp2 = get_maphash_list(local_State, tb_c1);
	if (mp == NULL)
	{
	    // There are no buffer-local mappings.
	    mp = mp2;
	    mp2 = NULL;
	}

	/*
	 * Loop until a partly matching mapping is found or all (local)
	 * mappings have been checked.
	 * The longest full match is remembered in "mp_match".
	 * A full match is only accepted if there is no partly match, so "aa"
	 * and "aaa" can both be mapped.
	 */
	mp_match = NULL;
	mp_match_len = 0;
	for ( ; mp != NULL;
	       mp->m_next == NULL ? (mp = mp2, mp2 = NULL) : (mp = mp->m_next))
	{
	    // Only consider an entry if the first character matches and it is
	    // for the current state.
	    // Skip ":lmap" mappings if keys were mapped.
	    if (mp->m_keys[0] == tb_c1
		    && (mp->m_mode & local_State)
		    && !(mp->m_simplified && seenModifyOtherKeys
						     && typebuf.tb_maplen == 0)
		    && ((mp->m_mode & LANGMAP) == 0 || typebuf.tb_maplen == 0))
	    {
#ifdef FEAT_LANGMAP
		int	nomap = nolmaplen;
		int	c2;
#endif
		// find the match length of this mapping
		for (mlen = 1; mlen < typebuf.tb_len; ++mlen)
		{
#ifdef FEAT_LANGMAP
		    c2 = typebuf.tb_buf[typebuf.tb_off + mlen];
		    if (nomap > 0)
			--nomap;
		    else if (c2 == K_SPECIAL)
			nomap = 2;
		    else
			LANGMAP_ADJUST(c2, TRUE);
		    if (mp->m_keys[mlen] != c2)
#else
		    if (mp->m_keys[mlen] !=
					 typebuf.tb_buf[typebuf.tb_off + mlen])
#endif
			break;
		}

		// Don't allow mapping the first byte(s) of a multi-byte char.
		// Happens when mapping <M-a> and then changing 'encoding'.
		// Beware that 0x80 is escaped.
		{
		    char_u *p1 = mp->m_keys;
		    char_u *p2 = mb_unescape(&p1);

		    if (has_mbyte && p2 != NULL
					&& MB_BYTE2LEN(tb_c1) > mb_ptr2len(p2))
			mlen = 0;
		}

		// Check an entry whether it matches.
		// - Full match: mlen == keylen
		// - Partly match: mlen == typebuf.tb_len
		keylen = mp->m_keylen;
		if (mlen == keylen || (mlen == typebuf.tb_len
						   && typebuf.tb_len < keylen))
		{
		    char_u  *s;
		    int	    n;

		    // If only script-local mappings are allowed, check if the
		    // mapping starts with K_SNR.
		    s = typebuf.tb_noremap + typebuf.tb_off;
		    if (*s == RM_SCRIPT
			    && (mp->m_keys[0] != K_SPECIAL
				|| mp->m_keys[1] != KS_EXTRA
				|| mp->m_keys[2] != KE_SNR))
			continue;

		    // If one of the typed keys cannot be remapped, skip the
		    // entry.
		    for (n = mlen; --n >= 0; )
			if (*s++ & (RM_NONE|RM_ABBR))
			    break;
		    if (n >= 0)
			continue;

		    if (keylen > typebuf.tb_len)
		    {
			if (!*timedout && !(mp_match != NULL
							&& mp_match->m_nowait))
			{
			    // break at a partly match
			    keylen = KEYLEN_PART_MAP;
			    break;
			}
		    }
		    else if (keylen > mp_match_len)
		    {
			// found a longer match
			mp_match = mp;
			mp_match_len = keylen;
		    }
		}
		else
		    // No match; may have to check for termcode at next
		    // character.
		    if (max_mlen < mlen)
			max_mlen = mlen;
	    }
	}

	// If no partly match found, use the longest full match.
	if (keylen != KEYLEN_PART_MAP)
	{
	    mp = mp_match;
	    keylen = mp_match_len;
	}
    }

    /*
     * Check for match with 'pastetoggle'
     */
    if (*p_pt != NUL && mp == NULL && (State & (INSERT|NORMAL)))
    {
	for (mlen = 0; mlen < typebuf.tb_len && p_pt[mlen]; ++mlen)
	    if (p_pt[mlen] != typebuf.tb_buf[typebuf.tb_off + mlen])
		    break;
	if (p_pt[mlen] == NUL)	// match
	{
	    // write chars to script file(s)
	    if (mlen > typebuf.tb_maplen)
		gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						     mlen - typebuf.tb_maplen);

	    del_typebuf(mlen, 0); // remove the chars
	    set_option_value((char_u *)"paste", (long)!p_paste, NULL, 0);
	    if (!(State & INSERT))
	    {
		msg_col = 0;
		msg_row = Rows - 1;
		msg_clr_eos();		// clear ruler
	    }
	    status_redraw_all();
	    redraw_statuslines();
	    showmode();
	    setcursor();
	    *keylenp = keylen;
	    return map_result_retry;
	}
	// Need more chars for partly match.
	if (mlen == typebuf.tb_len)
	    keylen = KEYLEN_PART_KEY;
	else if (max_mlen < mlen)
	    // no match, may have to check for termcode at next character
	    max_mlen = mlen + 1;
    }

    if ((mp == NULL || max_mlen >= mp_match_len) && keylen != KEYLEN_PART_MAP)
    {
	int	save_keylen = keylen;

	/*
	 * When no matching mapping found or found a non-matching mapping that
	 * matches at least what the matching mapping matched:
	 * Check if we have a terminal code, when:
	 * - mapping is allowed,
	 * - keys have not been mapped,
	 * - and not an ESC sequence, not in insert mode or p_ek is on,
	 * - and when not timed out,
	 */
	if ((no_mapping == 0 || allow_keys != 0)
		&& (typebuf.tb_maplen == 0
		    || (p_remap && typebuf.tb_noremap[
						    typebuf.tb_off] == RM_YES))
		&& !*timedout)
	{
	    keylen = check_termcode(max_mlen + 1, NULL, 0, NULL);

	    // If no termcode matched but 'pastetoggle' matched partially
	    // it's like an incomplete key sequence.
	    if (keylen == 0 && save_keylen == KEYLEN_PART_KEY)
		keylen = KEYLEN_PART_KEY;

	    // If no termcode matched, try to include the modifier into the
	    // key.  This is for when modifyOtherKeys is working.
	    if (keylen == 0 && !no_reduce_keys)
		keylen = check_simplify_modifier(max_mlen + 1);

	    // When getting a partial match, but the last characters were not
	    // typed, don't wait for a typed character to complete the
	    // termcode.  This helps a lot when a ":normal" command ends in an
	    // ESC.
	    if (keylen < 0 && typebuf.tb_len == typebuf.tb_maplen)
		keylen = 0;
	}
	else
	    keylen = 0;
	if (keylen == 0)	// no matching terminal code
	{
#ifdef AMIGA
	    // check for window bounds report
	    if (typebuf.tb_maplen == 0 && (typebuf.tb_buf[
						typebuf.tb_off] & 0xff) == CSI)
	    {
		char_u *s;

		for (s = typebuf.tb_buf + typebuf.tb_off + 1;
			   s < typebuf.tb_buf + typebuf.tb_off + typebuf.tb_len
		   && (VIM_ISDIGIT(*s) || *s == ';' || *s == ' ');
			++s)
		    ;
		if (*s == 'r' || *s == '|') // found one
		{
		    del_typebuf(
			  (int)(s + 1 - (typebuf.tb_buf + typebuf.tb_off)), 0);
		    // get size and redraw screen
		    shell_resized();
		    *keylenp = keylen;
		    return map_result_retry;
		}
		if (*s == NUL)	    // need more characters
		    keylen = KEYLEN_PART_KEY;
	    }
	    if (keylen >= 0)
#endif
		// When there was a matching mapping and no termcode could be
		// replaced after another one, use that mapping (loop around).
		// If there was no mapping at all use the character from the
		// typeahead buffer right here.
		if (mp == NULL)
		{
		    *keylenp = keylen;
		    return map_result_get;    // get character from typeahead
		}
	}

	if (keylen > 0)	    // full matching terminal code
	{
#if defined(FEAT_GUI) && defined(FEAT_MENU)
	    if (typebuf.tb_len >= 2
		    && typebuf.tb_buf[typebuf.tb_off] == K_SPECIAL
			      && typebuf.tb_buf[typebuf.tb_off + 1] == KS_MENU)
	    {
		int	idx;

		// Using a menu may cause a break in undo!  It's like using
		// gotchars(), but without recording or writing to a script
		// file.
		may_sync_undo();
		del_typebuf(3, 0);
		idx = get_menu_index(current_menu, local_State);
		if (idx != MENU_INDEX_INVALID)
		{
		    // In Select mode and a Visual mode menu is used:  Switch
		    // to Visual mode temporarily.  Append K_SELECT to switch
		    // back to Select mode.
		    if (VIsual_active && VIsual_select
					     && (current_menu->modes & VISUAL))
		    {
			VIsual_select = FALSE;
			(void)ins_typebuf(K_SELECT_STRING,
						   REMAP_NONE, 0, TRUE, FALSE);
		    }
		    ins_typebuf(current_menu->strings[idx],
				current_menu->noremap[idx],
				0, TRUE, current_menu->silent[idx]);
		}
	    }
#endif // FEAT_GUI && FEAT_MENU
	    *keylenp = keylen;
	    return map_result_retry;	// try mapping again
	}

	// Partial match: get some more characters.  When a matching mapping
	// was found use that one.
	if (mp == NULL || keylen < 0)
	    keylen = KEYLEN_PART_KEY;
	else
	    keylen = mp_match_len;
    }

    /*
     * complete match
     */
    if (keylen >= 0 && keylen <= typebuf.tb_len)
    {
	char_u *map_str;

#ifdef FEAT_EVAL
	int	save_m_expr;
	int	save_m_noremap;
	int	save_m_silent;
	char_u	*save_m_keys;
#else
# define save_m_noremap mp->m_noremap
# define save_m_silent mp->m_silent
#endif

	// write chars to script file(s)
	if (keylen > typebuf.tb_maplen)
	    gotchars(typebuf.tb_buf + typebuf.tb_off + typebuf.tb_maplen,
						   keylen - typebuf.tb_maplen);

	cmd_silent = (typebuf.tb_silent > 0);
	del_typebuf(keylen, 0);	// remove the mapped keys

	/*
	 * Put the replacement string in front of mapstr.
	 * The depth check catches ":map x y" and ":map y x".
	 */
	if (++*mapdepth >= p_mmd)
	{
	    emsg(_(e_recursive_mapping));
	    if (State & CMDLINE)
		redrawcmdline();
	    else
		setcursor();
	    flush_buffers(FLUSH_MINIMAL);
	    *mapdepth = 0;	// for next one
	    *keylenp = keylen;
	    return map_result_fail;
	}

	/*
	 * In Select mode and a Visual mode mapping is used: Switch to Visual
	 * mode temporarily.  Append K_SELECT to switch back to Select mode.
	 */
	if (VIsual_active && VIsual_select && (mp->m_mode & VISUAL))
	{
	    VIsual_select = FALSE;
	    (void)ins_typebuf(K_SELECT_STRING, REMAP_NONE, 0, TRUE, FALSE);
	}

#ifdef FEAT_EVAL
	// Copy the values from *mp that are used, because evaluating the
	// expression may invoke a function that redefines the mapping, thereby
	// making *mp invalid.
	save_m_expr = mp->m_expr;
	save_m_noremap = mp->m_noremap;
	save_m_silent = mp->m_silent;
	save_m_keys = NULL;  // only saved when needed

	/*
	 * Handle ":map <expr>": evaluate the {rhs} as an expression.  Also
	 * save and restore the command line for "normal :".
	 */
	if (mp->m_expr)
	{
	    int save_vgetc_busy = vgetc_busy;
	    int save_may_garbage_collect = may_garbage_collect;
	    int was_screen_col = screen_cur_col;
	    int was_screen_row = screen_cur_row;

	    vgetc_busy = 0;
	    may_garbage_collect = FALSE;

	    save_m_keys = vim_strsave(mp->m_keys);
	    map_str = eval_map_expr(mp, NUL);

	    // The mapping may do anything, but we expect it to take care of
	    // redrawing.  Do put the cursor back where it was.
	    windgoto(was_screen_row, was_screen_col);
	    out_flush();

	    vgetc_busy = save_vgetc_busy;
	    may_garbage_collect = save_may_garbage_collect;
	}
	else
#endif
	    map_str = mp->m_str;

	/*
	 * Insert the 'to' part in the typebuf.tb_buf.
	 * If 'from' field is the same as the start of the 'to' field, don't
	 * remap the first character (but do allow abbreviations).
	 * If m_noremap is set, don't remap the whole 'to' part.
	 */
	if (map_str == NULL)
	    i = FAIL;
	else
	{
	    int noremap;

	    if (save_m_noremap != REMAP_YES)
		noremap = save_m_noremap;
	    else if (
#ifdef FEAT_EVAL
		STRNCMP(map_str, save_m_keys != NULL ? save_m_keys : mp->m_keys,
								(size_t)keylen)
#else
		STRNCMP(map_str, mp->m_keys, (size_t)keylen)
#endif
		   != 0)
		noremap = REMAP_YES;
	    else
		noremap = REMAP_SKIP;
	    i = ins_typebuf(map_str, noremap,
					 0, TRUE, cmd_silent || save_m_silent);
#ifdef FEAT_EVAL
	    if (save_m_expr)
		vim_free(map_str);
	    last_used_map = mp;
#endif
	}
#ifdef FEAT_EVAL
	vim_free(save_m_keys);
#endif
	*keylenp = keylen;
	if (i == FAIL)
	    return map_result_fail;
	return map_result_retry;
    }

    *keylenp = keylen;
    return map_result_nomatch;
}