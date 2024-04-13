plot_command()
{
    plot_token = c_token++;
    plotted_data_from_stdin = FALSE;
    refresh_nplots = 0;
    SET_CURSOR_WAIT;
#ifdef USE_MOUSE
    plot_mode(MODE_PLOT);
    add_udv_by_name("MOUSE_X")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_Y")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_X2")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_Y2")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_BUTTON")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_SHIFT")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_ALT")->udv_value.type = NOTDEFINED;
    add_udv_by_name("MOUSE_CTRL")->udv_value.type = NOTDEFINED;
#endif
    plotrequest();
    /* Clear "hidden" flag for any plots that may have been toggled off */
    if (term->modify_plots)
	term->modify_plots(MODPLOTS_SET_VISIBLE, -1);
    SET_CURSOR_ARROW;
}