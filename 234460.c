check_for_mouse_events()
{
#ifdef USE_MOUSE
    if (term_initialised && term->waitforinput) {
	term->waitforinput(TERM_ONLY_CHECK_MOUSING);
    }
#endif
#ifdef _WIN32
    /* Process windows GUI events (e.g. for text window, or wxt and windows terminals) */
    WinMessageLoop();
    /* On Windows, Ctrl-C only sets this flag. */
    /* The next block duplicates the behaviour of inter(). */
    if (ctrlc_flag) {
	ctrlc_flag = FALSE;
	term_reset();
	putc('\n', stderr);
	fprintf(stderr, "Ctrl-C detected!\n");
	bail_to_command_line();	/* return to prompt */
    }
#endif
}