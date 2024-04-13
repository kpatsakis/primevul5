term_apply_lp_properties(struct lp_style_type *lp)
{
    /*  This function passes all the line and point properties to the
     *  terminal driver and issues the corresponding commands.
     *
     *  Alas, sometimes it might be necessary to give some help to
     *  this function by explicitly issuing additional '(*term)(...)'
     *  commands.
     */
    int lt = lp->l_type;
    int dt = lp->d_type;
    t_dashtype custom_dash_pattern = lp->custom_dash_pattern;
    t_colorspec colorspec = lp->pm3d_color;

    if ((lp->flags & LP_SHOW_POINTS)) {
	/* change points, too
	 * Currently, there is no 'pointtype' function.  For points
	 * there is a special function also dealing with (x,y) co-
	 * ordinates.
	 */
	if (lp->p_size < 0)
	    (*term->pointsize) (pointsize);
	else
	    (*term->pointsize) (lp->p_size);
    }
    /*  _first_ set the line width, _then_ set the line type !
     *  The linetype might depend on the linewidth in some terminals.
     */
    (*term->linewidth) (lp->l_width);

    /* LT_DEFAULT (used only by "set errorbars"?) means don't change it */
    if (lt == LT_DEFAULT)
	;
    else

    /* The paradigm for handling linetype and dashtype in version 5 is */
    /* linetype < 0 (e.g. LT_BACKGROUND, LT_NODRAW) means some special */
    /* category that will be handled directly by term->linetype().     */
    /* linetype > 0 is now redundant. It used to encode both a color   */
    /* and a dash pattern.  Now we have separate mechanisms for those. */
    if (LT_COLORFROMCOLUMN < lt && lt < 0)
	(*term->linetype) (lt);
    else if (term->set_color == null_set_color) {
	(*term->linetype) (lt-1);
	return;
    } else /* All normal lines will be solid unless a dashtype is given */
	(*term->linetype) (LT_SOLID);

    /* Version 5.3
     * If the line is not wanted at all, setting dashtype and color can only hurt
     */
    if (lt == LT_NODRAW)
	return;

    /* Apply dashtype or user-specified dash pattern, which may override  */
    /* the terminal-specific dot/dash pattern belonging to this linetype. */
    if (lt == LT_AXIS)
	; /* LT_AXIS is a special linetype that may incorporate a dash pattern */
    else if (dt == DASHTYPE_CUSTOM)
	(*term->dashtype) (dt, &custom_dash_pattern);
    else if (dt == DASHTYPE_SOLID)
	(*term->dashtype) (dt, NULL);
    else if (dt >= 0)
	/* The null_dashtype() routine or a version 5 terminal's private  */
	/* dashtype routine converts this into a call to term->linetype() */
	/* yielding the same result as in version 4 except possibly for a */
	/* different line width.					  */
	(*term->dashtype) (dt, NULL);

    /* Finally adjust the color of the line */
    apply_pm3dcolor(&colorspec);
}