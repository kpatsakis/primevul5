mb_str2wide(char_u *s)
{
    int		*res;
    char_u	*p;
    int		i = 0;

    res = (int *)alloc(sizeof(int) * (mb_charlen(s) + 1));
    if (res != NULL)
    {
	for (p = s; *p != NUL; )
	    res[i++] = mb_ptr2char_adv(&p);
	res[i] = NUL;
    }
    return res;
}