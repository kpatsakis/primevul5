static int __init con_init(void)
{
	const char *display_desc = NULL;
	struct vc_data *vc;
	unsigned int currcons = 0, i;

	console_lock();

	if (!conswitchp)
		conswitchp = &dummy_con;
	display_desc = conswitchp->con_startup();
	if (!display_desc) {
		fg_console = 0;
		console_unlock();
		return 0;
	}

	for (i = 0; i < MAX_NR_CON_DRIVER; i++) {
		struct con_driver *con_driver = &registered_con_driver[i];

		if (con_driver->con == NULL) {
			con_driver->con = conswitchp;
			con_driver->desc = display_desc;
			con_driver->flag = CON_DRIVER_FLAG_INIT;
			con_driver->first = 0;
			con_driver->last = MAX_NR_CONSOLES - 1;
			break;
		}
	}

	for (i = 0; i < MAX_NR_CONSOLES; i++)
		con_driver_map[i] = conswitchp;

	if (blankinterval) {
		blank_state = blank_normal_wait;
		mod_timer(&console_timer, jiffies + (blankinterval * HZ));
	}

	for (currcons = 0; currcons < MIN_NR_CONSOLES; currcons++) {
		vc_cons[currcons].d = vc = kzalloc(sizeof(struct vc_data), GFP_NOWAIT);
		INIT_WORK(&vc_cons[currcons].SAK_work, vc_SAK);
		tty_port_init(&vc->port);
		visual_init(vc, currcons, 1);
		vc->vc_screenbuf = kzalloc(vc->vc_screenbuf_size, GFP_NOWAIT);
		vc_init(vc, vc->vc_rows, vc->vc_cols,
			currcons || !vc->vc_sw->con_save_screen);
	}
	currcons = fg_console = 0;
	master_display_fg = vc = vc_cons[currcons].d;
	set_origin(vc);
	save_screen(vc);
	gotoxy(vc, vc->vc_x, vc->vc_y);
	csi_J(vc, 0);
	update_screen(vc);
	pr_info("Console: %s %s %dx%d\n",
		vc->vc_can_do_color ? "colour" : "mono",
		display_desc, vc->vc_cols, vc->vc_rows);
	printable = 1;

	console_unlock();

#ifdef CONFIG_VT_CONSOLE
	register_console(&vt_console_driver);
#endif
	return 0;
}