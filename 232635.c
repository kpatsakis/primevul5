static int emit_nops(u8 **pprog, int len)
{
	u8 *prog = *pprog;
	int i, noplen, cnt = 0;

	while (len > 0) {
		noplen = len;

		if (noplen > ASM_NOP_MAX)
			noplen = ASM_NOP_MAX;

		for (i = 0; i < noplen; i++)
			EMIT1(ideal_nops[noplen][i]);
		len -= noplen;
	}

	*pprog = prog;

	return cnt;
}