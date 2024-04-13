main (int argc, char *argv[])
{
    BufFilePtr	    inputraw, input, output;
    int		    c;
    
    inputraw = BufFileOpenRead (0);
    input = BufFilePushCompressed (inputraw);
    output = BufFileOpenWrite (1);
    while ((c = BufFileGet (input)) != BUFFILEEOF)
	BufFilePut (c, output);
    BufFileClose (input, FALSE);
    BufFileClose (output, FALSE);
    return 0;
}
