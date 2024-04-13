_PUBLIC_ bool islower_m(codepoint_t val)
{
	return (toupper_m(val) != val);
}
