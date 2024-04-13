do_string_replot(const char *s)
{
    do_string(s);

    if (volatile_data && (E_REFRESH_NOT_OK != refresh_ok)) {
	if (display_ipc_commands())
	    fprintf(stderr, "refresh\n");
	refresh_request();

    } else if (!replot_disabled)
	replotrequest();

    else
	int_warn(NO_CARET, "refresh not possible and replot is disabled");
}