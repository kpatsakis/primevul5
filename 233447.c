static void parse_path(char *p, char *q, char *id, int idsz)
{
	struct SYMBOL *s;
	char *buf, *r, *t, *op = NULL, *width, *scale, *trans;
	int i, fill, npar = 0;
	float x1, y1, x, y;
char *rmax;

	r = strstr(p, "class=\"");
	if (!r || r > q)
		return;
	r += 7;
	fill = strncmp(r, "fill", 4) == 0;
	width = strstr(p, "stroke-width:");
	scale = strstr(p, "scale(");
	if (scale && scale > q)
		scale = NULL;
	trans = strstr(p, "translate(");
	if (trans && trans > q)
		trans = NULL;
	for (;;) {
		p = strstr(p, "d=\"");
		if (!p)
			return;
		if (isspace((unsigned char) p[-1]))	// (check not 'id=..")
			break;
		p += 3;
	}
	i = (int) (q - p) * 4 + 200;		// estimated PS buffer size
	if (i > TEX_BUF_SZ)
		buf = malloc(i);
	else
		buf = tex_buf;
rmax=buf + i;
	r = buf;
	*r++ = '/';
	idsz -= 5;
	strncpy(r, id + 4, idsz);
	r += idsz;
	strcpy(r, "{gsave T ");
	r += strlen(r);
	if (scale || trans) {
		if (scale) {
			scale += 6;		// "scale("
			t = get_val(scale, &x1);
			if (*t == ',')
				t = get_val(t + 1, &y1);
			else
				y1 = x1;
		}
		if (trans) {
			trans += 10;		// "translate("
			t = get_val(trans, &x) + 1; //","
			t = get_val(t, &y);
		}
		if (!scale)
			r += sprintf(r, "%.2f %.2f T ", x, -y);
		else if (!trans)
			r += sprintf(r, "%.2f %.2f scale ", x1, y1);
		else if (scale > trans)
			r += sprintf(r, "%.2f %.2f T %.2f %.2f scale ",
					x, -y, x1, y1);
		else
			r += sprintf(r, "%.2f %.2f scale %.2f %.2f T ",
					x1, y1, x, -y);
	}
	strcpy(r, "0 0 M\n");
	r += strlen(r);
	if (width && width < q) {
		*r++ = ' ';
		width += 13;
		while (isdigit(*width) || *width == '.')
			*r++ = *width++;
		*r++ = ' ';
		*r++ = 'S';
		*r++ = 'L';
		*r++ = 'W';
	}
	p += 3;
	for (;;) {
		if (*p == '\0' || *p == '"')
			break;
		i = 0;
		switch (*p++) {
		default:
			if ((isdigit((unsigned char) p[-1]))
			 || p[-1] == '-' || p[-1] == '.') {
				if (!npar)
					continue;
				p--;			// same op
				break;
			}
			continue;
		case 'M':
			op = "M";
			npar = 2;
			break;
		case 'm':
			op = "RM";
			npar = 2;
			break;
		case 'L':
			op = "L";
			npar = 2;
			break;
		case 'l':
			op = "RL";
			npar = 2;
			break;
		case 'H':
			op = "H";
			npar = 1;
			break;
		case 'h':
			op = "h";
			npar = 1;
			break;
		case 'V':
			op = "V";
			npar = 1;
			break;
		case 'v':
			*r++ = ' ';
			*r++ = '0';
			op = "RL";
			i = 1;
			npar = 2;
			break;
		case 'z':
			op = "closepath";
			npar = 0;
			break;
		case 'C':
			op = "C";
			npar = 6;
			break;
		case 'c':
			op = "RC";
			npar = 6;
			break;
//		case 'A':
//			op = "arc";
//			break;
//		case 'a':
//			op = "arc";
//			break;
		case 'q':
			op = "RC";
			npar = 2;
			p = get_val(p, &x1);
			p = get_val(p, &y1);
			t = get_val(p, &x);
			t = get_val(t, &y);
			r += sprintf(r, " %.2f %.2f %.2f %.2f",
					x1*2/3, -y1*2/3,
					x1+(x-x1)*2/3, -y1-(y-y1)*2/3);
			break;
		case 't':
			op = "RC";
			npar = 2;
			x1 = x - x1;
			y1 = y - y1;
			t = get_val(p, &x);
			t = get_val(t, &y);
			r += sprintf(r, " %.2f %.2f %.2f %.2f",
					x1*2/3, -y1*2/3,
					x1+(x-x1)*2/3, -y1-(y-y1)*2/3);
			break;
		}
		*r++ = ' ';
		for ( ; i < npar; i++) {
			while (isspace((unsigned char) *p))
				p++;
			if (i & 1) {		// y is inverted
				if (*p == '-')
					p++;
				else if (*p != '0' || p[1] != ' ')
					*r++ = '-';
			}
			while ((isdigit((unsigned char) *p))
			    || *p == '-' || *p == '.')
				*r++ = *p++;
			*r++ = ' ';
		}
		if (*op == 'h') {
			*r++ = '0';
			*r++ = ' ';
			op = "RL";
		}
		strcpy(r, op);
		r += strlen(r);
if (r + 30 > rmax) bug("Buffer overflow in SVG to PS", 1);
	}
	strcpy(r, fill ? " fill" : " stroke");
	r += strlen(r);
	strcpy(r, "\ngrestore}!");
	r += strlen(r);

	s = getarena(sizeof(struct SYMBOL));
	memset(s, 0, sizeof(struct SYMBOL));
	s->text = getarena(strlen(buf) + 1);
	strcpy(s->text, buf);
	ps_def(s, s->text, 'p');
	if (buf != tex_buf)
		free(buf);
}