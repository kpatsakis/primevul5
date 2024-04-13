term_end_multiplot()
{
    FPRINTF((stderr, "term_end_multiplot()\n"));
    if (!multiplot)
	return;

    if (term_suspended) {
	if (term->resume)
	    (*term->resume) ();
	term_suspended = FALSE;
    }

    multiplot_end();

    term_end_plot();
#ifdef USE_MOUSE
    UpdateStatusline();
#endif
}