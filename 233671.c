static int utf8_unichar_valid_range(int unichar)
{
	if (unichar > 0x10ffff)
		return 0;
	if ((unichar & 0xfffff800) == 0xd800)
		return 0;
	if ((unichar > 0xfdcf) && (unichar < 0xfdf0))
		return 0;
	if ((unichar & 0xffff) == 0xffff)
		return 0;
	return 1;
}