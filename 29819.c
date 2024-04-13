_PUBLIC_ int codepoint_cmpi(codepoint_t c1, codepoint_t c2)
{
	if (c1 == c2 ||
	    toupper_m(c1) == toupper_m(c2)) {
		return 0;
	}
	return c1 - c2;
}
