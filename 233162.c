offset2bytes(int nr, char_u *buf)
{
    int	    rem;
    int	    b1, b2, b3, b4;

    /* Split the number in parts of base 255.  We need to avoid NUL bytes. */
    b1 = nr % 255 + 1;
    rem = nr / 255;
    b2 = rem % 255 + 1;
    rem = rem / 255;
    b3 = rem % 255 + 1;
    b4 = rem / 255 + 1;

    if (b4 > 1 || b3 > 0x1f)	/* 4 bytes */
    {
	buf[0] = 0xe0 + b4;
	buf[1] = b3;
	buf[2] = b2;
	buf[3] = b1;
	return 4;
    }
    if (b3 > 1 || b2 > 0x3f )	/* 3 bytes */
    {
	buf[0] = 0xc0 + b3;
	buf[1] = b2;
	buf[2] = b1;
	return 3;
    }
    if (b2 > 1 || b1 > 0x7f )	/* 2 bytes */
    {
	buf[0] = 0x80 + b2;
	buf[1] = b1;
	return 2;
    }
				/* 1 byte */
    buf[0] = b1;
    return 1;
}