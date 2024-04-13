sug_maketable(spellinfo_T *spin)
{
    garray_T	ga;
    int		res = OK;

    /* Allocate a buffer, open a memline for it and create the swap file
     * (uses a temp file, not a .swp file). */
    spin->si_spellbuf = open_spellbuf();
    if (spin->si_spellbuf == NULL)
	return FAIL;

    /* Use a buffer to store the line info, avoids allocating many small
     * pieces of memory. */
    ga_init2(&ga, 1, 100);

    /* recursively go through the tree */
    if (sug_filltable(spin, spin->si_foldroot->wn_sibling, 0, &ga) == -1)
	res = FAIL;

    ga_clear(&ga);
    return res;
}