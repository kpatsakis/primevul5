_PUBLIC_ codepoint_t toupper_m(codepoint_t val)
{
	if (val < 128) {
		return toupper(val);
	}
	if (val >= ARRAY_SIZE(upcase_table)) {
		return val;
	}
	return upcase_table[val];
}
