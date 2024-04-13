estimate_strlen(const char *text, double *height)
{
    int len;
    char *s;
    double estimated_fontheight = 1.0;

    if ((term->flags & TERM_IS_LATEX))
	return strlen_tex(text);

#ifdef GP_ENH_EST
    if (strchr(text,'\n') || (term->flags & TERM_ENHANCED_TEXT)) {
	struct termentry *tsave = term;
	term = &ENHest;
	term->put_text(0,0,text);
	len = term->xmax;
	estimated_fontheight = term->ymax / 10.;
	term = tsave;
	/* Assume that unicode escape sequences  \U+xxxx will generate a single character */
	/* ENHest_plaintext is filled in by the put_text() call to estimate.trm           */
	s = ENHest_plaintext;
	while ((s = contains_unicode(s)) != NULL) {
	    len -= 6;
	    s += 6;
	}
	FPRINTF((stderr,"Estimating length %d height %g for enhanced text \"%s\"",
		len, estimated_fontheight, text));
	FPRINTF((stderr,"  plain text \"%s\"\n", ENHest_plaintext));
    } else if (encoding == S_ENC_UTF8)
	len = strwidth_utf8(text);
    else
#endif
	len = strlen(text);

    if (height)
	*height = estimated_fontheight;

    return len;
}