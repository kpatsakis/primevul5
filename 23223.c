ebcdic2ascii(char_u *buffer, int len)
{
    int		i;

    for (i = 0; i < len; i++)
	buffer[i] = ebcdic2ascii_tab[buffer[i]];
}