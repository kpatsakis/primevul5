BufCompressedSkip (BufFilePtr f, int bytes)
{
    int		    c;
    while (bytes--) 
    {
	c = BufFileGet(f);
	if (c == BUFFILEEOF)
	    return BUFFILEEOF;
    }
    return 0;
}
