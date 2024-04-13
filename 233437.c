static void gch_capo(struct SYMBOL *s)
{
	char *p = s->text, *q, *r;
	int i, l, li = 0;
	static const char *capo_txt = "  (capo: %d)";
	static signed char cap_trans[] =
		{0, 5, -2, 3, -4, 1, -6, -1, 4, -3, 2, -5};

	// search the chord symbols
	for (;;) {
		if (!strchr("^_<>@", *p))
			break;
		p = strchr(p, '\n');
		if (!p)
			return;
		p++;
	}

	// add a capo chord symbol
	i = p - s->text;
	q = strchr(p + 1, '\n');
	if (q)
		l = q - p;
	else
		l = strlen(p);
	if (!capo) {
		capo = 1;
		li = strlen(capo_txt);
	}
	r = (char *) getarena(strlen(s->text) + l + li + 1);
	i += l;
	strncpy(r, s->text, i);		// annotations + chord symbol
	r[i++] = '\n';
	strncpy(r + i, p, l);		// capo
	if (li) {
		sprintf(r + i + l, capo_txt, cfmt.capo);
		l += li;
	}
	if (q)
		strcpy(r + i + l, q);	// ending annotations
	s->text = r;
	gch_tr1(s, i, cap_trans[cfmt.capo % 12]);
}