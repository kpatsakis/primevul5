vgetc(void)
{
    int		c, c2;
    int		n;
    char_u	buf[MB_MAXBYTES + 1];
    int		i;

#ifdef FEAT_EVAL
    // Do garbage collection when garbagecollect() was called previously and
    // we are now at the toplevel.
    if (may_garbage_collect && want_garbage_collect)
	garbage_collect(FALSE);
#endif

    /*
     * If a character was put back with vungetc, it was already processed.
     * Return it directly.
     */
    if (old_char != -1)
    {
	c = old_char;
	old_char = -1;
	mod_mask = old_mod_mask;
	mouse_row = old_mouse_row;
	mouse_col = old_mouse_col;
    }
    else
    {
	mod_mask = 0;
	vgetc_mod_mask = 0;
	vgetc_char = 0;
	last_recorded_len = 0;

	for (;;)		// this is done twice if there are modifiers
	{
	    int did_inc = FALSE;

	    if (mod_mask
#if defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
		    || im_is_preediting()
#endif
#if defined(FEAT_PROP_POPUP)
		    || popup_no_mapping()
#endif
		    )
	    {
		// no mapping after modifier has been read
		++no_mapping;
		++allow_keys;
		did_inc = TRUE;	// mod_mask may change value
	    }
	    c = vgetorpeek(TRUE);
	    if (did_inc)
	    {
		--no_mapping;
		--allow_keys;
	    }

	    // Get two extra bytes for special keys
	    if (c == K_SPECIAL
#ifdef FEAT_GUI
		    || (c == CSI)
#endif
	       )
	    {
		int	    save_allow_keys = allow_keys;

		++no_mapping;
		allow_keys = 0;		// make sure BS is not found
		c2 = vgetorpeek(TRUE);	// no mapping for these chars
		c = vgetorpeek(TRUE);
		--no_mapping;
		allow_keys = save_allow_keys;
		if (c2 == KS_MODIFIER)
		{
		    mod_mask = c;
		    continue;
		}
		c = TO_SPECIAL(c2, c);

#if defined(FEAT_GUI_MSWIN) && defined(FEAT_MENU) && defined(FEAT_TEAROFF)
		// Handle K_TEAROFF here, the caller of vgetc() doesn't need to
		// know that a menu was torn off
		if (
# ifdef VIMDLL
		    gui.in_use &&
# endif
		    c == K_TEAROFF)
		{
		    char_u	name[200];
		    int		i;

		    // get menu path, it ends with a <CR>
		    for (i = 0; (c = vgetorpeek(TRUE)) != '\r'; )
		    {
			name[i] = c;
			if (i < 199)
			    ++i;
		    }
		    name[i] = NUL;
		    gui_make_tearoff(name);
		    continue;
		}
#endif
#if defined(FEAT_GUI) && defined(FEAT_GUI_GTK) && defined(FEAT_MENU)
		// GTK: <F10> normally selects the menu, but it's passed until
		// here to allow mapping it.  Intercept and invoke the GTK
		// behavior if it's not mapped.
		if (c == K_F10 && gui.menubar != NULL)
		{
		    gtk_menu_shell_select_first(
					   GTK_MENU_SHELL(gui.menubar), FALSE);
		    continue;
		}
#endif
#ifdef FEAT_GUI
		// Handle focus event here, so that the caller doesn't need to
		// know about it.  Return K_IGNORE so that we loop once (needed
		// if 'lazyredraw' is set).
		if (c == K_FOCUSGAINED || c == K_FOCUSLOST)
		{
		    ui_focus_change(c == K_FOCUSGAINED);
		    c = K_IGNORE;
		}

		// Translate K_CSI to CSI.  The special key is only used to
		// avoid it being recognized as the start of a special key.
		if (c == K_CSI)
		    c = CSI;
#endif
	    }
	    // a keypad or special function key was not mapped, use it like
	    // its ASCII equivalent
	    switch (c)
	    {
		case K_KPLUS:	c = '+'; break;
		case K_KMINUS:	c = '-'; break;
		case K_KDIVIDE:	c = '/'; break;
		case K_KMULTIPLY: c = '*'; break;
		case K_KENTER:	c = CAR; break;
		case K_KPOINT:
#ifdef MSWIN
				// Can be either '.' or a ',',
				// depending on the type of keypad.
				c = MapVirtualKey(VK_DECIMAL, 2); break;
#else
				c = '.'; break;
#endif
		case K_K0:	c = '0'; break;
		case K_K1:	c = '1'; break;
		case K_K2:	c = '2'; break;
		case K_K3:	c = '3'; break;
		case K_K4:	c = '4'; break;
		case K_K5:	c = '5'; break;
		case K_K6:	c = '6'; break;
		case K_K7:	c = '7'; break;
		case K_K8:	c = '8'; break;
		case K_K9:	c = '9'; break;

		case K_XHOME:
		case K_ZHOME:	if (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_HOME;
				    mod_mask = 0;
				}
				else if (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_HOME;
				    mod_mask = 0;
				}
				else
				    c = K_HOME;
				break;
		case K_XEND:
		case K_ZEND:	if (mod_mask == MOD_MASK_SHIFT)
				{
				    c = K_S_END;
				    mod_mask = 0;
				}
				else if (mod_mask == MOD_MASK_CTRL)
				{
				    c = K_C_END;
				    mod_mask = 0;
				}
				else
				    c = K_END;
				break;

		case K_XUP:	c = K_UP; break;
		case K_XDOWN:	c = K_DOWN; break;
		case K_XLEFT:	c = K_LEFT; break;
		case K_XRIGHT:	c = K_RIGHT; break;
	    }

	    // For a multi-byte character get all the bytes and return the
	    // converted character.
	    // Note: This will loop until enough bytes are received!
	    if (has_mbyte && (n = MB_BYTE2LEN_CHECK(c)) > 1)
	    {
		++no_mapping;
		buf[0] = c;
		for (i = 1; i < n; ++i)
		{
		    buf[i] = vgetorpeek(TRUE);
		    if (buf[i] == K_SPECIAL
#ifdef FEAT_GUI
			    || (buf[i] == CSI)
#endif
			    )
		    {
			// Must be a K_SPECIAL - KS_SPECIAL - KE_FILLER
			// sequence, which represents a K_SPECIAL (0x80),
			// or a CSI - KS_EXTRA - KE_CSI sequence, which
			// represents a CSI (0x9B),
			// or a K_SPECIAL - KS_EXTRA - KE_CSI, which is CSI
			// too.
			c = vgetorpeek(TRUE);
			if (vgetorpeek(TRUE) == KE_CSI && c == KS_EXTRA)
			    buf[i] = CSI;
		    }
		}
		--no_mapping;
		c = (*mb_ptr2char)(buf);
	    }

	    if (vgetc_char == 0)
	    {
		vgetc_mod_mask = mod_mask;
		vgetc_char = c;
	    }

	    break;
	}
    }

#ifdef FEAT_EVAL
    /*
     * In the main loop "may_garbage_collect" can be set to do garbage
     * collection in the first next vgetc().  It's disabled after that to
     * avoid internally used Lists and Dicts to be freed.
     */
    may_garbage_collect = FALSE;
#endif

#ifdef FEAT_BEVAL_TERM
    if (c != K_MOUSEMOVE && c != K_IGNORE && c != K_CURSORHOLD)
    {
	// Don't trigger 'balloonexpr' unless only the mouse was moved.
	bevalexpr_due_set = FALSE;
	ui_remove_balloon();
    }
#endif
#ifdef FEAT_PROP_POPUP
    // Only filter keys that do not come from ":normal".  Keys from feedkeys()
    // are filtered.
    if ((!ex_normal_busy || in_feedkeys) && popup_do_filter(c))
    {
	if (c == Ctrl_C)
	    got_int = FALSE;  // avoid looping
	c = K_IGNORE;
    }
#endif

    // Need to process the character before we know it's safe to do something
    // else.
    if (c != K_IGNORE)
	state_no_longer_safe("key typed");

    return c;
}