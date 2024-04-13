term_suspend()
{
    FPRINTF((stderr, "term_suspend()\n"));
    if (term_initialised && !term_suspended && term->suspend) {
	FPRINTF((stderr, "- calling term->suspend()\n"));
	(*term->suspend) ();
	term_suspended = TRUE;
    }
}