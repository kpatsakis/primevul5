getroom(
    spellinfo_T *spin,
    size_t	len,		/* length needed */
    int		align)		/* align for pointer */
{
    char_u	*p;
    sblock_T	*bl = spin->si_blocks;

    if (align && bl != NULL)
	/* Round size up for alignment.  On some systems structures need to be
	 * aligned to the size of a pointer (e.g., SPARC). */
	bl->sb_used = (bl->sb_used + sizeof(char *) - 1)
						      & ~(sizeof(char *) - 1);

    if (bl == NULL || bl->sb_used + len > SBLOCKSIZE)
    {
	if (len >= SBLOCKSIZE)
	    bl = NULL;
	else
	    /* Allocate a block of memory. It is not freed until much later. */
	    bl = (sblock_T *)alloc_clear(
				   (unsigned)(sizeof(sblock_T) + SBLOCKSIZE));
	if (bl == NULL)
	{
	    if (!spin->si_did_emsg)
	    {
		EMSG(_("E845: Insufficient memory, word list will be incomplete"));
		spin->si_did_emsg = TRUE;
	    }
	    return NULL;
	}
	bl->sb_next = spin->si_blocks;
	spin->si_blocks = bl;
	bl->sb_used = 0;
	++spin->si_blocks_cnt;
    }

    p = bl->sb_data + bl->sb_used;
    bl->sb_used += (int)len;

    return p;
}