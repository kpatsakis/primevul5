get_nfd(uint32_t *cp1, uint32_t *cp2, uint32_t uc)
{
	int t, b;

	/*
	 * These are not converted to NFD on Mac OS.
	 */
	if ((uc >= 0x2000 && uc <= 0x2FFF) ||
	    (uc >= 0xF900 && uc <= 0xFAFF) ||
	    (uc >= 0x2F800 && uc <= 0x2FAFF))
		return (0);
	/*
	 * Those code points are not converted to NFD on Mac OS.
	 * I do not know the reason because it is undocumented.
	 *   NFC        NFD
	 *   1109A  ==> 11099 110BA
	 *   1109C  ==> 1109B 110BA
	 *   110AB  ==> 110A5 110BA
	 */
	if (uc == 0x1109A || uc == 0x1109C || uc == 0x110AB)
		return (0);

	t = 0;
	b = sizeof(u_decomposition_table)/sizeof(u_decomposition_table[0]) -1;
	while (b >= t) {
		int m = (t + b) / 2;
		if (u_decomposition_table[m].nfc < uc)
			t = m + 1;
		else if (u_decomposition_table[m].nfc > uc)
			b = m - 1;
		else {
			*cp1 = u_decomposition_table[m].cp1;
			*cp2 = u_decomposition_table[m].cp2;
			return (1);
		}
	}
	return (0);
}