vte_sequence_handler_decset_internal(VteTerminal *terminal,
				     int setting,
				     gboolean restore,
				     gboolean save,
				     gboolean set)
{
	gboolean recognized = FALSE;
	gpointer p;
	guint i;
	struct {
		int setting;
		gboolean *bvalue;
		gint *ivalue;
		gpointer *pvalue;
		gpointer fvalue;
		gpointer tvalue;
		VteTerminalSequenceHandler reset, set;
	} settings[] = {
		/* 1: Application/normal cursor keys. */
		{1, NULL, &terminal->pvt->cursor_mode, NULL,
		 GINT_TO_POINTER(VTE_KEYMODE_NORMAL),
		 GINT_TO_POINTER(VTE_KEYMODE_APPLICATION),
		 NULL, NULL,},
		/* 2: disallowed, we don't do VT52. */
		{2, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 3: disallowed, window size is set by user. */
		{3, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 4: Smooth scroll. */
		{4, &terminal->pvt->smooth_scroll, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 5: Reverse video. */
		{5, &terminal->pvt->screen->reverse_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 6: Origin mode: when enabled, cursor positioning is
		 * relative to the scrolling region. */
		{6, &terminal->pvt->screen->origin_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 7: Wraparound mode. */
		{7, &terminal->pvt->flags.am, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 8: disallowed, keyboard repeat is set by user. */
		{8, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 9: Send-coords-on-click. */
		{9, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_SEND_XY_ON_CLICK),
		 NULL, NULL,},
		/* 12: disallowed, cursor blinks is set by user. */
		{12, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 18: print form feed. */
		/* 19: set print extent to full screen. */
		/* 25: Cursor visible. */
		{25, &terminal->pvt->cursor_visible, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 30/rxvt: disallowed, scrollbar visibility is set by user. */
		{30, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 35/rxvt: disallowed, fonts set by user. */
		{35, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 38: enter Tektronix mode. */
		/* 40: disallowed, the user sizes dynamically. */
		{40, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 41: more(1) fix. */
		/* 42: Enable NLS replacements. */
		{42, &terminal->pvt->nrc_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 44: Margin bell. */
		{44, &terminal->pvt->margin_bell, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 47: Alternate screen. */
		{47, NULL, NULL, (gpointer) &terminal->pvt->screen,
		 &terminal->pvt->normal_screen,
		 &terminal->pvt->alternate_screen,
		 NULL, NULL,},
		/* 66: Keypad mode. */
		{66, &terminal->pvt->keypad_mode, NULL, NULL,
		 GINT_TO_POINTER(VTE_KEYMODE_NORMAL),
		 GINT_TO_POINTER(VTE_KEYMODE_APPLICATION),
		 NULL, NULL,},
		/* 67: disallowed, backspace key policy is set by user. */
		{67, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1000: Send-coords-on-button. */
		{1000, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_SEND_XY_ON_BUTTON),
		 NULL, NULL,},
		/* 1001: Hilite tracking. */
		{1001, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_HILITE_TRACKING),
		 NULL, NULL,},
		/* 1002: Cell motion tracking. */
		{1002, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_CELL_MOTION_TRACKING),
		 NULL, NULL,},
		/* 1003: All motion tracking. */
		{1003, NULL, &terminal->pvt->mouse_tracking_mode, NULL,
		 GINT_TO_POINTER(0),
		 GINT_TO_POINTER(MOUSE_TRACKING_ALL_MOTION_TRACKING),
		 NULL, NULL,},
		/* 1010/rxvt: disallowed, scroll-on-output is set by user. */
		{1010, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1011/rxvt: disallowed, scroll-on-keypress is set by user. */
		{1011, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1035: disallowed, don't know what to do with it. */
		{1035, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1036: Meta-sends-escape. */
		{1036, &terminal->pvt->meta_sends_escape, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
		/* 1037: disallowed, delete key policy is set by user. */
		{1037, NULL, NULL, NULL, NULL, NULL, NULL, NULL,},
		/* 1047: Use alternate screen buffer. */
		{1047, NULL, NULL, (gpointer) &terminal->pvt->screen,
		 &terminal->pvt->normal_screen,
		 &terminal->pvt->alternate_screen,
		 NULL, NULL,},
		/* 1048: Save/restore cursor position. */
		{1048, NULL, NULL, NULL,
		 NULL,
		 NULL,
		 vte_sequence_handler_rc,
		 vte_sequence_handler_sc,},
		/* 1049: Use alternate screen buffer, saving the cursor
		 * position. */
		{1049, NULL, NULL, (gpointer) &terminal->pvt->screen,
		 &terminal->pvt->normal_screen,
		 &terminal->pvt->alternate_screen,
		 vte_sequence_handler_rc,
		 vte_sequence_handler_sc,},
		/* 1051: Sun function key mode. */
		{1051, NULL, NULL, (gpointer) &terminal->pvt->sun_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1052: HP function key mode. */
		{1052, NULL, NULL, (gpointer) &terminal->pvt->hp_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1060: Legacy function key mode. */
		{1060, NULL, NULL, (gpointer) &terminal->pvt->legacy_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 1061: VT220 function key mode. */
		{1061, NULL, NULL, (gpointer) &terminal->pvt->vt220_fkey_mode,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL},
		/* 2004: Bracketed paste mode. */
		{2004, &terminal->pvt->screen->bracketed_paste_mode, NULL, NULL,
		 GINT_TO_POINTER(FALSE),
		 GINT_TO_POINTER(TRUE),
		 NULL, NULL,},
	};

	/* Handle the setting. */
	for (i = 0; i < G_N_ELEMENTS(settings); i++)
	if (settings[i].setting == setting) {
		recognized = TRUE;
		/* Handle settings we want to ignore. */
		if ((settings[i].fvalue == settings[i].tvalue) &&
		    (settings[i].set == NULL) &&
		    (settings[i].reset == NULL)) {
			continue;
		}

		/* Read the old setting. */
		if (restore) {
			p = g_hash_table_lookup(terminal->pvt->dec_saved,
						GINT_TO_POINTER(setting));
			set = (p != NULL);
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d was %s.\n",
					setting, set ? "set" : "unset");
		}
		/* Save the current setting. */
		if (save) {
			if (settings[i].bvalue) {
				set = *(settings[i].bvalue) != FALSE;
			} else
			if (settings[i].ivalue) {
				set = *(settings[i].ivalue) ==
				      GPOINTER_TO_INT(settings[i].tvalue);
			} else
			if (settings[i].pvalue) {
				set = *(settings[i].pvalue) ==
				      settings[i].tvalue;
			}
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d is %s, saving.\n",
					setting, set ? "set" : "unset");
			g_hash_table_insert(terminal->pvt->dec_saved,
					    GINT_TO_POINTER(setting),
					    GINT_TO_POINTER(set));
		}
		/* Change the current setting to match the new/saved value. */
		if (!save) {
			_vte_debug_print(VTE_DEBUG_PARSE,
					"Setting %d to %s.\n",
					setting, set ? "set" : "unset");
			if (settings[i].set && set) {
				settings[i].set (terminal, NULL);
			}
			if (settings[i].bvalue) {
				*(settings[i].bvalue) = set;
			} else
			if (settings[i].ivalue) {
				*(settings[i].ivalue) = set ?
					GPOINTER_TO_INT(settings[i].tvalue) :
					GPOINTER_TO_INT(settings[i].fvalue);
			} else
			if (settings[i].pvalue) {
				*(settings[i].pvalue) = set ?
					settings[i].tvalue :
					settings[i].fvalue;
			}
			if (settings[i].reset && !set) {
				settings[i].reset (terminal, NULL);
			}
		}
	}

	/* Do whatever's necessary when the setting changes. */
	switch (setting) {
	case 1:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering application cursor mode.\n" :
				"Leaving application cursor mode.\n");
		break;
#if 0		/* 3: disallowed, window size is set by user. */
	case 3:
		vte_terminal_emit_resize_window(terminal,
						(set ? 132 : 80) *
						terminal->char_width +
						terminal->pvt->inner_border.left +
                                                terminal->pvt->inner_border.right,
						terminal->row_count *
						terminal->char_height +
						terminal->pvt->inner_border.top +
                                                terminal->pvt->inner_border.bottom);
		/* Request a resize and redraw. */
		_vte_invalidate_all(terminal);
		break;
#endif
	case 5:
		/* Repaint everything in reverse mode. */
		_vte_invalidate_all(terminal);
		break;
	case 6:
		/* Reposition the cursor in its new home position. */
		terminal->pvt->screen->cursor_current.col = 0;
		terminal->pvt->screen->cursor_current.row =
			terminal->pvt->screen->insert_delta;
		break;
	case 47:
	case 1047:
	case 1049:
		/* Clear the alternate screen if we're switching
		 * to it, and home the cursor. */
		if (set) {
			_vte_terminal_clear_screen (terminal);
			_vte_terminal_home_cursor (terminal);
		}
		/* Reset scrollbars and repaint everything. */
		gtk_adjustment_set_value(terminal->adjustment,
					 terminal->pvt->screen->scroll_delta);
		vte_terminal_set_scrollback_lines(terminal,
				terminal->pvt->scrollback_lines);
		_vte_terminal_queue_contents_changed(terminal);
		_vte_invalidate_all (terminal);
		break;
	case 9:
	case 1000:
	case 1001:
	case 1002:
	case 1003:
		/* Make the pointer visible. */
		_vte_terminal_set_pointer_visible(terminal, TRUE);
		break;
	case 66:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering application keypad mode.\n" :
				"Leaving application keypad mode.\n");
		break;
	case 1051:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering Sun fkey mode.\n" :
				"Leaving Sun fkey mode.\n");
		break;
	case 1052:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering HP fkey mode.\n" :
				"Leaving HP fkey mode.\n");
		break;
	case 1060:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering Legacy fkey mode.\n" :
				"Leaving Legacy fkey mode.\n");
		break;
	case 1061:
		_vte_debug_print(VTE_DEBUG_KEYBOARD, set ?
				"Entering VT220 fkey mode.\n" :
				"Leaving VT220 fkey mode.\n");
		break;
	default:
		break;
	}

	if (!recognized) {
		_vte_debug_print (VTE_DEBUG_MISC,
				  "DECSET/DECRESET mode %d not recognized, ignoring.\n",
				  setting);
	}
}