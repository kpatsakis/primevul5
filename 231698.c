BOOL clear_context_reset(CLEAR_CONTEXT* clear)
{
	if (!clear)
		return FALSE;

	clear->seqNumber = 0;
	return TRUE;
}