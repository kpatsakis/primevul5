refresh_request()
{
    AXIS_INDEX axis;

    if (   ((first_plot == NULL) && (refresh_ok == E_REFRESH_OK_2D))
	|| ((first_3dplot == NULL) && (refresh_ok == E_REFRESH_OK_3D))
	|| (!*replot_line && (refresh_ok == E_REFRESH_NOT_OK))
       )
	int_error(NO_CARET, "no active plot; cannot refresh");

    if (refresh_ok == E_REFRESH_NOT_OK) {
	int_warn(NO_CARET, "cannot refresh from this state. trying full replot");
	replotrequest();
	return;
    }

    /* The margins from "set offset" were already applied;
     * don't reapply them here
     */
    retain_offsets = TRUE;

    /* Restore the axis range/scaling state from original plot
     * Dima Kogan April 2018
     */
    for (axis = 0; axis < NUMBER_OF_MAIN_VISIBLE_AXES; axis++) {
	AXIS *this_axis = &axis_array[axis];
	if ((this_axis->set_autoscale & AUTOSCALE_MIN)
	&&  (this_axis->writeback_min < VERYLARGE))
	    this_axis->set_min = this_axis->writeback_min;
	else
	    this_axis->min = this_axis->set_min;
	if ((this_axis->set_autoscale & AUTOSCALE_MAX)
	&&  (this_axis->writeback_max > -VERYLARGE))
	    this_axis->set_max = this_axis->writeback_max;
	else
	    this_axis->max = this_axis->set_max;

	if (this_axis->linked_to_secondary)
	    clone_linked_axes(this_axis, this_axis->linked_to_secondary);
	else if (this_axis->linked_to_primary) {
	    if (this_axis->linked_to_primary->autoscale != AUTOSCALE_BOTH)
	    clone_linked_axes(this_axis, this_axis->linked_to_primary);
	}
    }

    if (refresh_ok == E_REFRESH_OK_2D) {
	refresh_bounds(first_plot, refresh_nplots);
	do_plot(first_plot, refresh_nplots);
	update_gpval_variables(1);
    } else if (refresh_ok == E_REFRESH_OK_3D) {
	refresh_3dbounds(first_3dplot, refresh_nplots);
	do_3dplot(first_3dplot, refresh_nplots, 0);
	update_gpval_variables(1);
    } else
	int_error(NO_CARET, "Internal error - refresh of unknown plot type");

}