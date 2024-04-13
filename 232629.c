has_cursorhold(void)
{
    return (first_autopat[(int)(get_real_state() == NORMAL_BUSY
			    ? EVENT_CURSORHOLD : EVENT_CURSORHOLDI)] != NULL);
}