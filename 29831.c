_PUBLIC_ codepoint_t tolower_m(codepoint_t val)
{
	if (val < 128) {
		return tolower(val);
	}
	if (val >= ARRAY_SIZE(lowcase_table)) {
		return val;
	}
	return lowcase_table[val];
}
