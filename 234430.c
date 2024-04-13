toggle_command()
{
    int plotno = -1;
    char *plottitle = NULL;
    TBOOLEAN foundit = FALSE;

    c_token++;

    if (equals(c_token, "all")) {
	c_token++;

    } else if ((plottitle = try_to_get_string()) != NULL) {
	struct curve_points *plot;
	int length = strlen(plottitle);
	if (refresh_ok == E_REFRESH_OK_2D)
	    plot = first_plot;
	else if (refresh_ok == E_REFRESH_OK_3D)
	    plot = (struct curve_points *)first_3dplot;
	else
	    plot = NULL;
	for (plotno = 0; plot != NULL; plot = plot->next, plotno++) {
	    if (plot->title)
		if (!strcmp(plot->title, plottitle)
		||  (plottitle[length-1] == '*' && !strncmp(plot->title, plottitle, length-1))) {
		    foundit = TRUE;
		    break;
		}
	}
	free(plottitle);
	if (!foundit) {
	    int_warn(NO_CARET,"Did not find a plot with that title");
	    return;
	}

    } else {
	plotno = int_expression() - 1;
    }

    if (term->modify_plots)
	term->modify_plots(MODPLOTS_INVERT_VISIBILITIES, plotno);
}