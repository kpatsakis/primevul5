change_term(const char *origname, int length)
{
    int i;
    struct termentry *t = NULL;
    TBOOLEAN ambiguous = FALSE;

    /* For backwards compatibility only */
    char *name = (char *)origname;
    if (!strncmp(origname,"X11",length)) {
	name = "x11";
	length = 3;
    }

#ifdef HAVE_CAIROPDF
    /* To allow "set term eps" as short for "set term epscairo" */
    if (!strncmp(origname,"eps",length)) {
	name = "epscairo";
	length = 8;
    }
#endif
#ifdef HAVE_LIBGD
    /* To allow "set term sixel" as short for "set term sixelgd" */
    if (!strncmp(origname, "sixel", length)) {
	name = "sixelgd";
	length = 7;
    }
#endif

    for (i = 0; i < TERMCOUNT; i++) {
	if (!strncmp(name, term_tbl[i].name, length)) {
	    if (t)
		ambiguous = TRUE;
	    t = term_tbl + i;
	    /* Exact match is always accepted */
	    if (length == strlen(term_tbl[i].name)) {
		ambiguous = FALSE;
		break;
	    }
	}
    }

    if (!t || ambiguous)
	return (NULL);

    /* Success: set terminal type now */

    term = t;
    term_initialised = FALSE;

    /* check that optional fields are initialised to something */
    if (term->text_angle == 0)
	term->text_angle = null_text_angle;
    if (term->justify_text == 0)
	term->justify_text = null_justify_text;
    if (term->point == 0)
	term->point = do_point;
    if (term->arrow == 0)
	term->arrow = do_arrow;
    if (term->pointsize == 0)
	term->pointsize = do_pointsize;
    if (term->linewidth == 0)
	term->linewidth = null_linewidth;
    if (term->layer == 0)
	term->layer = null_layer;
    if (term->tscale <= 0)
	term->tscale = 1.0;
    if (term->set_font == 0)
	term->set_font = null_set_font;
    if (term->set_color == 0) {
	term->set_color = null_set_color;
	term->flags |= TERM_NULL_SET_COLOR;
    }
    if (term->dashtype == 0)
	term->dashtype = null_dashtype;

    if (interactive)
	fprintf(stderr, "\nTerminal type is now '%s'\n", term->name);

    /* Invalidate any terminal-specific structures that may be active */
    invalidate_palette();

    return (t);
}