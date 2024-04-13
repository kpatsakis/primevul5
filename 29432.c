BufFilePushCompressed (BufFilePtr f)
{
    int		    code;
    int		    maxbits;
    int		    hsize;
    CompressedFile  *file;
    int		    extra;

    if ((BufFileGet(f) != (magic_header[0] & 0xFF)) ||
	(BufFileGet(f) != (magic_header[1] & 0xFF)))
    {
	return 0;
    }
    code = BufFileGet (f);
    if (code == BUFFILEEOF) return 0;
    
    maxbits = code & BIT_MASK;
    if (maxbits > BITS || maxbits < 12)
	return 0;
    hsize = hsize_table[maxbits - 12];
    extra = (1 << maxbits) * sizeof (char_type) +
	    hsize * sizeof (unsigned short);
    file = malloc (sizeof (CompressedFile) + extra);
    if (!file)
	return 0;
    file->file = f;
    file->maxbits = maxbits;
    file->block_compress = code & BLOCK_MASK;
    file->maxmaxcode = 1 << file->maxbits;
    file->tab_suffix = (char_type *) &file[1];
    file->tab_prefix = (unsigned short *) (file->tab_suffix + file->maxmaxcode);
    /*
     * As above, initialize the first 256 entries in the table.
     */
    file->maxcode = MAXCODE(file->n_bits = INIT_BITS);
    for ( code = 255; code >= 0; code-- ) {
	file->tab_prefix[code] = 0;
	file->tab_suffix[code] = (char_type) code;
    }
    file->free_ent = ((file->block_compress) ? FIRST : 256 );
    file->clear_flg = 0;
    file->offset = 0;
    file->size = 0;
    file->stackp = file->de_stack;
    bzero(file->buf, BITS);
    file->finchar = file->oldcode = getcode (file);
    if (file->oldcode != -1)
	*file->stackp++ = file->finchar;
    return BufFileCreate ((char *) file,
			  BufCompressedFill,
			  0,
			  BufCompressedSkip,
			  BufCompressedClose);
}
