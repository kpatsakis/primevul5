static int name_from_dns(struct address buf[static MAXADDRS], char canon[static 256], const char *name, int family, const struct resolvconf *conf)
{
	unsigned char qbuf[2][280], abuf[2][512];
	const unsigned char *qp[2] = { qbuf[0], qbuf[1] };
	unsigned char *ap[2] = { abuf[0], abuf[1] };
	int qlens[2], alens[2];
	int i, nq = 0;
	struct dpc_ctx ctx = { .addrs = buf, .canon = canon };
	static const struct { int af; int rr; } afrr[2] = {
		{ .af = AF_INET6, .rr = RR_A },
		{ .af = AF_INET, .rr = RR_AAAA },
	};

	for (i=0; i<2; i++) {
		if (family != afrr[i].af) {
			qlens[nq] = __res_mkquery(0, name, 1, afrr[i].rr,
				0, 0, 0, qbuf[nq], sizeof *qbuf);
			if (qlens[nq] == -1)
				return EAI_NONAME;
			nq++;
		}
	}

	if (__res_msend_rc(nq, qp, qlens, ap, alens, sizeof *abuf, conf) < 0)
		return EAI_SYSTEM;

	for (i=0; i<nq; i++)
		__dns_parse(abuf[i], alens[i], dns_parse_callback, &ctx);

	if (ctx.cnt) return ctx.cnt;
	if (alens[0] < 4 || (abuf[0][3] & 15) == 2) return EAI_AGAIN;
	if ((abuf[0][3] & 15) == 0) return EAI_NONAME;
	if ((abuf[0][3] & 15) == 3) return 0;
	return EAI_FAIL;
}
