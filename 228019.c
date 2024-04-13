char_avail(void)
{
    int	    retval;

#ifdef FEAT_EVAL
    // When test_override("char_avail", 1) was called pretend there is no
    // typeahead.
    if (disable_char_avail_for_testing)
	return FALSE;
#endif
    ++no_mapping;
    retval = vpeekc();
    --no_mapping;
    return (retval != NUL);
}