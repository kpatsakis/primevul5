term_start_plot()
{
    FPRINTF((stderr, "term_start_plot()\n"));

    if (!term_initialised)
	term_initialise();

    if (!term_graphics) {
	FPRINTF((stderr, "- calling term->graphics()\n"));
	(*term->graphics) ();
	term_graphics = TRUE;
    } else if (multiplot && term_suspended) {
	if (term->resume) {
	    FPRINTF((stderr, "- calling term->resume()\n"));
	    (*term->resume) ();
	}
	term_suspended = FALSE;
    }

    if (multiplot)
	multiplot_count++;

    /* Sync point for epslatex text positioning */
    (*term->layer)(TERM_LAYER_RESET);

    /* Because PostScript plots may be viewed out of order, make sure */
    /* Each new plot makes no assumption about the previous palette.  */
    if (term->flags & TERM_IS_POSTSCRIPT)
	invalidate_palette();

    /* Set canvas size to full range of current terminal coordinates */
	canvas.xleft  = 0;
	canvas.xright = term->xmax - 1;
	canvas.ybot   = 0;
	canvas.ytop   = term->ymax - 1;

}