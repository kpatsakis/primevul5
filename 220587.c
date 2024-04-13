handle_did_throw()
{
    char	*p = NULL;
    msglist_T	*messages = NULL;
    ESTACK_CHECK_DECLARATION

    /*
     * If the uncaught exception is a user exception, report it as an
     * error.  If it is an error exception, display the saved error
     * message now.  For an interrupt exception, do nothing; the
     * interrupt message is given elsewhere.
     */
    switch (current_exception->type)
    {
	case ET_USER:
	    vim_snprintf((char *)IObuff, IOSIZE,
		    _("E605: Exception not caught: %s"),
		    current_exception->value);
	    p = (char *)vim_strsave(IObuff);
	    break;
	case ET_ERROR:
	    messages = current_exception->messages;
	    current_exception->messages = NULL;
	    break;
	case ET_INTERRUPT:
	    break;
    }

    estack_push(ETYPE_EXCEPT, current_exception->throw_name,
					current_exception->throw_lnum);
    ESTACK_CHECK_SETUP
    current_exception->throw_name = NULL;

    discard_current_exception();	// uses IObuff if 'verbose'
    suppress_errthrow = TRUE;
    force_abort = TRUE;

    if (messages != NULL)
    {
	do
	{
	    msglist_T	*next = messages->next;
	    int		save_compiling = estack_compiling;

	    estack_compiling = messages->msg_compiling;
	    emsg(messages->msg);
	    vim_free(messages->msg);
	    vim_free(messages->sfile);
	    vim_free(messages);
	    messages = next;
	    estack_compiling = save_compiling;
	}
	while (messages != NULL);
    }
    else if (p != NULL)
    {
	emsg(p);
	vim_free(p);
    }
    vim_free(SOURCING_NAME);
    ESTACK_CHECK_NOW
    estack_pop();
}