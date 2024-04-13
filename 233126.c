add_fromto(
    spellinfo_T	*spin,
    garray_T	*gap,
    char_u	*from,
    char_u	*to)
{
    fromto_T	*ftp;
    char_u	word[MAXWLEN];

    if (ga_grow(gap, 1) == OK)
    {
	ftp = ((fromto_T *)gap->ga_data) + gap->ga_len;
	(void)spell_casefold(from, (int)STRLEN(from), word, MAXWLEN);
	ftp->ft_from = getroom_save(spin, word);
	(void)spell_casefold(to, (int)STRLEN(to), word, MAXWLEN);
	ftp->ft_to = getroom_save(spin, word);
	++gap->ga_len;
    }
}