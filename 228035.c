stuffReadbuffSpec(char_u *s)
{
    int c;

    while (*s != NUL)
    {
	if (*s == K_SPECIAL && s[1] != NUL && s[2] != NUL)
	{
	    // Insert special key literally.
	    stuffReadbuffLen(s, 3L);
	    s += 3;
	}
	else
	{
	    c = mb_ptr2char_adv(&s);
	    if (c == CAR || c == NL || c == ESC)
		c = ' ';
	    stuffcharReadbuff(c);
	}
    }
}