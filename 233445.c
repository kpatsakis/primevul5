static void gch_transpose(struct SYMBOL *s)
{
	int in_ch = 0;
	int i2 = curvoice->ckey.sf - curvoice->okey.sf;
	char *o = s->text, *p = o;

	// search the chord symbols
	for (;;) {
		if (in_ch || !strchr("^_<>@", *p)) {
			gch_tr1(s, p - s->text, i2);
			p = s->text + (p - o);
			o = s->text;
			for (p++; *p; p++) {
				if (strchr("\t;\n", *p))
					break;
			}
			if (!*p)
				break;
			switch (*p) {
			case '\t':
				in_ch = 1;
				break;
			case ';':
				in_ch = !strchr("^_<>@", p[1]);
				break;
			default:
				in_ch = 0;
				break;
			}
		} else {
			p = strchr(p, '\n');
			if (!p)
				break;
		}
		p++;
	}
}