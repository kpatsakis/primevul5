check_simplify_modifier(int max_offset)
{
    int		offset;
    char_u	*tp;

    for (offset = 0; offset < max_offset; ++offset)
    {
	if (offset + 3 >= typebuf.tb_len)
	    break;
	tp = typebuf.tb_buf + typebuf.tb_off + offset;
	if (tp[0] == K_SPECIAL && tp[1] == KS_MODIFIER)
	{
	    // A modifier was not used for a mapping, apply it to ASCII keys.
	    // Shift would already have been applied.
	    int modifier = tp[2];
	    int	c = tp[3];
	    int new_c = merge_modifyOtherKeys(c, &modifier);

	    if (new_c != c)
	    {
		char_u	new_string[MB_MAXBYTES];
		int	len;

		if (offset == 0)
		{
		    // At the start: remember the character and mod_mask before
		    // merging, in some cases, e.g. at the hit-return prompt,
		    // they are put back in the typeahead buffer.
		    vgetc_char = c;
		    vgetc_mod_mask = tp[2];
		}
		len = mb_char2bytes(new_c, new_string);
		if (modifier == 0)
		{
		    if (put_string_in_typebuf(offset, 4, new_string, len,
							   NULL, 0, 0) == FAIL)
		    return -1;
		}
		else
		{
		    tp[2] = modifier;
		    if (put_string_in_typebuf(offset + 3, 1, new_string, len,
							   NULL, 0, 0) == FAIL)
		    return -1;
		}
		return len;
	    }
	}
    }
    return 0;
}