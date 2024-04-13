write_multiline(
    int x, int y,
    char *text,
    JUSTIFY hor,                /* horizontal ... */
    VERT_JUSTIFY vert,          /* ... and vertical just - text in hor direction despite angle */
    int angle,                  /* assume term has already been set for this */
    const char *font)           /* NULL or "" means use default */
{
    struct termentry *t = term;
    char *p = text;

    if (!p)
	return;

    /* EAM 9-Feb-2003 - Set font before calculating sizes */
    if (font && *font)
	(*t->set_font) (font);

    if (vert != JUST_TOP) {
	/* count lines and adjust y */
	int lines = 0;          /* number of linefeeds - one fewer than lines */
	while (*p) {
	    if (*p++ == '\n')
		++lines;
	}
	if (angle)
	    x -= (vert * lines * t->v_char) / 2;
	else
	    y += (vert * lines * t->v_char) / 2;
    }

    for (;;) {                  /* we will explicitly break out */

	if ((text != NULL) && (p = strchr(text, '\n')) != NULL)
	    *p = 0;             /* terminate the string */

	if ((*t->justify_text) (hor)) {
	    if (on_page(x, y))
		(*t->put_text) (x, y, text);
	} else {
	    int len = estimate_strlen(text, NULL);
	    int hfix, vfix;

	    if (angle == 0) {
		hfix = hor * t->h_char * len / 2;
		vfix = 0;
	    } else {
		/* Attention: This relies on the numeric values of enum JUSTIFY! */
		hfix = hor * t->h_char * len * cos(angle * DEG2RAD) / 2 + 0.5;
		vfix = hor * t->v_char * len * sin(angle * DEG2RAD) / 2 + 0.5;
	    }
		if (on_page(x - hfix, y - vfix))
		    (*t->put_text) (x - hfix, y - vfix, text);
	}
	if (angle == 90 || angle == TEXT_VERTICAL)
	    x += t->v_char;
	else if (angle == -90 || angle == -TEXT_VERTICAL)
	    x -= t->v_char;
	else
	    y -= t->v_char;

	if (!p)
	    break;
	else {
	    /* put it back */
	    *p = '\n';
	}

	text = p + 1;
    }                           /* unconditional branch back to the for(;;) - just a goto ! */

    if (font && *font)
	(*t->set_font) ("");

}