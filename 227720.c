static unsigned int uvc_print_terms(struct list_head *terms, u16 dir,
		char *buffer)
{
	struct uvc_entity *term;
	unsigned int nterms = 0;
	char *p = buffer;

	list_for_each_entry(term, terms, chain) {
		if (!UVC_ENTITY_IS_TERM(term) ||
		    UVC_TERM_DIRECTION(term) != dir)
			continue;

		if (nterms)
			p += sprintf(p, ",");
		if (++nterms >= 4) {
			p += sprintf(p, "...");
			break;
		}
		p += sprintf(p, "%u", term->id);
	}

	return p - buffer;
}