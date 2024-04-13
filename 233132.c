spell_print_node(wordnode_T *node, int depth)
{
    if (node->wn_u1.index)
    {
	/* Done this node before, print the reference. */
	PRINTSOME(line1, depth, "(%d)", node->wn_nr, 0);
	PRINTSOME(line2, depth, "    ", 0, 0);
	PRINTSOME(line3, depth, "    ", 0, 0);
	msg((char_u *)line1);
	msg((char_u *)line2);
	msg((char_u *)line3);
    }
    else
    {
	node->wn_u1.index = TRUE;

	if (node->wn_byte != NUL)
	{
	    if (node->wn_child != NULL)
		PRINTSOME(line1, depth, " %c -> ", node->wn_byte, 0);
	    else
		/* Cannot happen? */
		PRINTSOME(line1, depth, " %c ???", node->wn_byte, 0);
	}
	else
	    PRINTSOME(line1, depth, " $    ", 0, 0);

	PRINTSOME(line2, depth, "%d/%d    ", node->wn_nr, node->wn_refs);

	if (node->wn_sibling != NULL)
	    PRINTSOME(line3, depth, " |    ", 0, 0);
	else
	    PRINTSOME(line3, depth, "      ", 0, 0);

	if (node->wn_byte == NUL)
	{
	    msg((char_u *)line1);
	    msg((char_u *)line2);
	    msg((char_u *)line3);
	}

	/* do the children */
	if (node->wn_byte != NUL && node->wn_child != NULL)
	    spell_print_node(node->wn_child, depth + 1);

	/* do the siblings */
	if (node->wn_sibling != NULL)
	{
	    /* get rid of all parent details except | */
	    STRCPY(line1, line3);
	    STRCPY(line2, line3);
	    spell_print_node(node->wn_sibling, depth);
	}
    }
}