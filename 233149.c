spell_make_sugfile(spellinfo_T *spin, char_u *wfname)
{
    char_u	*fname = NULL;
    int		len;
    slang_T	*slang;
    int		free_slang = FALSE;

    /*
     * Read back the .spl file that was written.  This fills the required
     * info for soundfolding.  This also uses less memory than the
     * pointer-linked version of the trie.  And it avoids having two versions
     * of the code for the soundfolding stuff.
     * It might have been done already by spell_reload_one().
     */
    for (slang = first_lang; slang != NULL; slang = slang->sl_next)
	if (fullpathcmp(wfname, slang->sl_fname, FALSE) == FPC_SAME)
	    break;
    if (slang == NULL)
    {
	spell_message(spin, (char_u *)_("Reading back spell file..."));
	slang = spell_load_file(wfname, NULL, NULL, FALSE);
	if (slang == NULL)
	    return;
	free_slang = TRUE;
    }

    /*
     * Clear the info in "spin" that is used.
     */
    spin->si_blocks = NULL;
    spin->si_blocks_cnt = 0;
    spin->si_compress_cnt = 0;	    /* will stay at 0 all the time*/
    spin->si_free_count = 0;
    spin->si_first_free = NULL;
    spin->si_foldwcount = 0;

    /*
     * Go through the trie of good words, soundfold each word and add it to
     * the soundfold trie.
     */
    spell_message(spin, (char_u *)_("Performing soundfolding..."));
    if (sug_filltree(spin, slang) == FAIL)
	goto theend;

    /*
     * Create the table which links each soundfold word with a list of the
     * good words it may come from.  Creates buffer "spin->si_spellbuf".
     * This also removes the wordnr from the NUL byte entries to make
     * compression possible.
     */
    if (sug_maketable(spin) == FAIL)
	goto theend;

    smsg((char_u *)_("Number of words after soundfolding: %ld"),
				 (long)spin->si_spellbuf->b_ml.ml_line_count);

    /*
     * Compress the soundfold trie.
     */
    spell_message(spin, (char_u *)_(msg_compressing));
    wordtree_compress(spin, spin->si_foldroot);

    /*
     * Write the .sug file.
     * Make the file name by changing ".spl" to ".sug".
     */
    fname = alloc(MAXPATHL);
    if (fname == NULL)
	goto theend;
    vim_strncpy(fname, wfname, MAXPATHL - 1);
    len = (int)STRLEN(fname);
    fname[len - 2] = 'u';
    fname[len - 1] = 'g';
    sug_write(spin, fname);

theend:
    vim_free(fname);
    if (free_slang)
	slang_free(slang);
    free_blocks(spin->si_blocks);
    close_spellbuf(spin->si_spellbuf);
}