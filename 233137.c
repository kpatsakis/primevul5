sug_write(spellinfo_T *spin, char_u *fname)
{
    FILE	*fd;
    wordnode_T	*tree;
    int		nodecount;
    int		wcount;
    char_u	*line;
    linenr_T	lnum;
    int		len;

    /* Create the file.  Note that an existing file is silently overwritten! */
    fd = mch_fopen((char *)fname, "w");
    if (fd == NULL)
    {
	EMSG2(_(e_notopen), fname);
	return;
    }

    vim_snprintf((char *)IObuff, IOSIZE,
				  _("Writing suggestion file %s ..."), fname);
    spell_message(spin, IObuff);

    /*
     * <SUGHEADER>: <fileID> <versionnr> <timestamp>
     */
    if (fwrite(VIMSUGMAGIC, VIMSUGMAGICL, (size_t)1, fd) != 1) /* <fileID> */
    {
	EMSG(_(e_write));
	goto theend;
    }
    putc(VIMSUGVERSION, fd);				/* <versionnr> */

    /* Write si_sugtime to the file. */
    put_time(fd, spin->si_sugtime);			/* <timestamp> */

    /*
     * <SUGWORDTREE>
     */
    spin->si_memtot = 0;
    tree = spin->si_foldroot->wn_sibling;

    /* Clear the index and wnode fields in the tree. */
    clear_node(tree);

    /* Count the number of nodes.  Needed to be able to allocate the
     * memory when reading the nodes.  Also fills in index for shared
     * nodes. */
    nodecount = put_node(NULL, tree, 0, 0, FALSE);

    /* number of nodes in 4 bytes */
    put_bytes(fd, (long_u)nodecount, 4);	/* <nodecount> */
    spin->si_memtot += nodecount + nodecount * sizeof(int);

    /* Write the nodes. */
    (void)put_node(fd, tree, 0, 0, FALSE);

    /*
     * <SUGTABLE>: <sugwcount> <sugline> ...
     */
    wcount = spin->si_spellbuf->b_ml.ml_line_count;
    put_bytes(fd, (long_u)wcount, 4);	/* <sugwcount> */

    for (lnum = 1; lnum <= (linenr_T)wcount; ++lnum)
    {
	/* <sugline>: <sugnr> ... NUL */
	line = ml_get_buf(spin->si_spellbuf, lnum, FALSE);
	len = (int)STRLEN(line) + 1;
	if (fwrite(line, (size_t)len, (size_t)1, fd) == 0)
	{
	    EMSG(_(e_write));
	    goto theend;
	}
	spin->si_memtot += len;
    }

    /* Write another byte to check for errors. */
    if (putc(0, fd) == EOF)
	EMSG(_(e_write));

    vim_snprintf((char *)IObuff, IOSIZE,
		 _("Estimated runtime memory use: %d bytes"), spin->si_memtot);
    spell_message(spin, IObuff);

theend:
    /* close the file */
    fclose(fd);
}