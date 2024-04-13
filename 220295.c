get_nfc(uint32_t uc, uint32_t uc2)
{
	int t, b;

	t = 0;
	b = sizeof(u_composition_table)/sizeof(u_composition_table[0]) -1;
	while (b >= t) {
		int m = (t + b) / 2;
		if (u_composition_table[m].cp1 < uc)
			t = m + 1;
		else if (u_composition_table[m].cp1 > uc)
			b = m - 1;
		else if (u_composition_table[m].cp2 < uc2)
			t = m + 1;
		else if (u_composition_table[m].cp2 > uc2)
			b = m - 1;
		else
			return (u_composition_table[m].nfc);
	}
	return (0);
}