static int vc_do_resize(struct tty_struct *tty, struct vc_data *vc,
				unsigned int cols, unsigned int lines)
{
	unsigned long old_origin, new_origin, new_scr_end, rlth, rrem, err = 0;
	unsigned long end;
	unsigned int old_rows, old_row_size, first_copied_row;
	unsigned int new_cols, new_rows, new_row_size, new_screen_size;
	unsigned int user;
	unsigned short *newscreen;
	struct uni_screen *new_uniscr = NULL;

	WARN_CONSOLE_UNLOCKED();

	if (!vc)
		return -ENXIO;

	user = vc->vc_resize_user;
	vc->vc_resize_user = 0;

	if (cols > VC_RESIZE_MAXCOL || lines > VC_RESIZE_MAXROW)
		return -EINVAL;

	new_cols = (cols ? cols : vc->vc_cols);
	new_rows = (lines ? lines : vc->vc_rows);
	new_row_size = new_cols << 1;
	new_screen_size = new_row_size * new_rows;

	if (new_cols == vc->vc_cols && new_rows == vc->vc_rows)
		return 0;

	if (new_screen_size > (4 << 20))
		return -EINVAL;
	newscreen = kzalloc(new_screen_size, GFP_USER);
	if (!newscreen)
		return -ENOMEM;

	if (get_vc_uniscr(vc)) {
		new_uniscr = vc_uniscr_alloc(new_cols, new_rows);
		if (!new_uniscr) {
			kfree(newscreen);
			return -ENOMEM;
		}
	}

	if (vc_is_sel(vc))
		clear_selection();

	old_rows = vc->vc_rows;
	old_row_size = vc->vc_size_row;

	err = resize_screen(vc, new_cols, new_rows, user);
	if (err) {
		kfree(newscreen);
		kfree(new_uniscr);
		return err;
	}

	vc->vc_rows = new_rows;
	vc->vc_cols = new_cols;
	vc->vc_size_row = new_row_size;
	vc->vc_screenbuf_size = new_screen_size;

	rlth = min(old_row_size, new_row_size);
	rrem = new_row_size - rlth;
	old_origin = vc->vc_origin;
	new_origin = (long) newscreen;
	new_scr_end = new_origin + new_screen_size;

	if (vc->vc_y > new_rows) {
		if (old_rows - vc->vc_y < new_rows) {
			/*
			 * Cursor near the bottom, copy contents from the
			 * bottom of buffer
			 */
			first_copied_row = (old_rows - new_rows);
		} else {
			/*
			 * Cursor is in no man's land, copy 1/2 screenful
			 * from the top and bottom of cursor position
			 */
			first_copied_row = (vc->vc_y - new_rows/2);
		}
		old_origin += first_copied_row * old_row_size;
	} else
		first_copied_row = 0;
	end = old_origin + old_row_size * min(old_rows, new_rows);

	vc_uniscr_copy_area(new_uniscr, new_cols, new_rows,
			    get_vc_uniscr(vc), rlth/2, first_copied_row,
			    min(old_rows, new_rows));
	vc_uniscr_set(vc, new_uniscr);

	update_attr(vc);

	while (old_origin < end) {
		scr_memcpyw((unsigned short *) new_origin,
			    (unsigned short *) old_origin, rlth);
		if (rrem)
			scr_memsetw((void *)(new_origin + rlth),
				    vc->vc_video_erase_char, rrem);
		old_origin += old_row_size;
		new_origin += new_row_size;
	}
	if (new_scr_end > new_origin)
		scr_memsetw((void *)new_origin, vc->vc_video_erase_char,
			    new_scr_end - new_origin);
	kfree(vc->vc_screenbuf);
	vc->vc_screenbuf = newscreen;
	vc->vc_screenbuf_size = new_screen_size;
	set_origin(vc);

	/* do part of a reset_terminal() */
	vc->vc_top = 0;
	vc->vc_bottom = vc->vc_rows;
	gotoxy(vc, vc->vc_x, vc->vc_y);
	save_cur(vc);

	if (tty) {
		/* Rewrite the requested winsize data with the actual
		   resulting sizes */
		struct winsize ws;
		memset(&ws, 0, sizeof(ws));
		ws.ws_row = vc->vc_rows;
		ws.ws_col = vc->vc_cols;
		ws.ws_ypixel = vc->vc_scan_lines;
		tty_do_resize(tty, &ws);
	}

	if (con_is_visible(vc))
		update_screen(vc);
	vt_event_post(VT_EVENT_RESIZE, vc->vc_num, vc->vc_num);
	notify_update(vc);
	return err;
}