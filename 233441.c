static void parse_defs(char *p, char *q)
{
	char *id, *r;
	int idsz;

	for (;;) {
		id = strstr(p, "id=\"");
		if (!id || id > q)
			return;
		r = strchr(id + 4, '"');
		if (!r)
			return;
		idsz = r + 1 - id;

		// if SVG output, mark the id as defined
		if (svg || epsf > 1) {
			svg_def_id(id, idsz);
			p = r;
			continue;
		}

		// convert SVG to PS
		p = id;
		while (*p != '<')
			p--;
		if (strncmp(p, "<path ", 6) == 0) {
			r = strstr(p, "/>");
			parse_path(p + 6, r, id, idsz);
			if (!r)
				break;
			p = r + 2;
			continue;
		}
		break;
	}
}