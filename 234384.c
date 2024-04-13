raise_lower_command(int lower)
{
    ++c_token;

    if (END_OF_COMMAND) {
	if (lower) {
#ifdef OS2
	    pm_lower_terminal_window();
#endif
#ifdef X11
	    x11_lower_terminal_group();
#endif
#ifdef _WIN32
	    win_lower_terminal_group();
#endif
#ifdef WXWIDGETS
	    wxt_lower_terminal_group();
#endif
	} else {
#ifdef OS2
	    pm_raise_terminal_window();
#endif
#ifdef X11
	    x11_raise_terminal_group();
#endif
#ifdef _WIN32
	    win_raise_terminal_group();
#endif
#ifdef WXWIDGETS
	    wxt_raise_terminal_group();
#endif
	}
	return;
    } else {
	int number;
	int negative = equals(c_token, "-");

	if (negative || equals(c_token, "+")) c_token++;
	if (!END_OF_COMMAND && isanumber(c_token)) {
	    number = real_expression();
	    if (negative)
	    number = -number;
	    if (lower) {
#ifdef OS2
		pm_lower_terminal_window();
#endif
#ifdef X11
		x11_lower_terminal_window(number);
#endif
#ifdef _WIN32
		win_lower_terminal_window(number);
#endif
#ifdef WXWIDGETS
		wxt_lower_terminal_window(number);
#endif
	    } else {
#ifdef OS2
		pm_raise_terminal_window();
#endif
#ifdef X11
		x11_raise_terminal_window(number);
#endif
#ifdef _WIN32
		win_raise_terminal_window(number);
#endif
#ifdef WXWIDGETS
		wxt_raise_terminal_window(number);
#endif
	    }
	    ++c_token;
	    return;
	}
    }
    if (lower)
	int_error(c_token, "usage: lower {plot_id}");
    else
	int_error(c_token, "usage: raise {plot_id}");
}