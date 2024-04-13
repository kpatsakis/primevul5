ldns_rr_new_frm_fp_l(ldns_rr **newrr, FILE *fp, uint32_t *default_ttl, ldns_rdf **origin, ldns_rdf **prev, int *line_nr)
{
	char *line;
	const char *endptr;  /* unused */
	ldns_rr *rr;
	uint32_t ttl;
	ldns_rdf *tmp;
	ldns_status s;
	ssize_t size;

	if (default_ttl) {
		ttl = *default_ttl;
	} else {
		ttl = 0;
	}

	line = LDNS_XMALLOC(char, LDNS_MAX_LINELEN + 1);
	if (!line) {
		return LDNS_STATUS_MEM_ERR;
	}

	/* read an entire line in from the file */
	if ((size = ldns_fget_token_l(fp, line, LDNS_PARSE_SKIP_SPACE, LDNS_MAX_LINELEN, line_nr)) == -1) {
		LDNS_FREE(line);
		/* if last line was empty, we are now at feof, which is not
		 * always a parse error (happens when for instance last line
		 * was a comment)
		 */
		return LDNS_STATUS_SYNTAX_ERR;
	}

	/* we can have the situation, where we've read ok, but still got
	 * no bytes to play with, in this case size is 0
	 */
	if (size == 0) {
		LDNS_FREE(line);
		return LDNS_STATUS_SYNTAX_EMPTY;
	}

	if (strncmp(line, "$ORIGIN", 7) == 0 && isspace((unsigned char)line[7])) {
		if (*origin) {
			ldns_rdf_deep_free(*origin);
			*origin = NULL;
		}
		tmp = ldns_rdf_new_frm_str(LDNS_RDF_TYPE_DNAME,
				ldns_strip_ws(line + 8));
		if (!tmp) {
			/* could not parse what next to $ORIGIN */
			LDNS_FREE(line);
			return LDNS_STATUS_SYNTAX_DNAME_ERR;
		}
		*origin = tmp;
		s = LDNS_STATUS_SYNTAX_ORIGIN;
	} else if (strncmp(line, "$TTL", 4) == 0 && isspace((unsigned char)line[4])) {
		if (default_ttl) {
			*default_ttl = ldns_str2period(
					ldns_strip_ws(line + 5), &endptr);
		}
		s = LDNS_STATUS_SYNTAX_TTL;
	} else if (strncmp(line, "$INCLUDE", 8) == 0) {
		s = LDNS_STATUS_SYNTAX_INCLUDE;
	} else if (!*ldns_strip_ws(line)) {
		LDNS_FREE(line);
		return LDNS_STATUS_SYNTAX_EMPTY;
	} else {
		if (origin && *origin) {
			s = ldns_rr_new_frm_str(&rr, (const char*) line, ttl, *origin, prev);
		} else {
			s = ldns_rr_new_frm_str(&rr, (const char*) line, ttl, NULL, prev);
		}
	}
	LDNS_FREE(line);
	if (s == LDNS_STATUS_OK) {
		if (newrr) {
			*newrr = rr;
		} else {
			/* Just testing if it would parse? */
			ldns_rr_free(rr);
		}
	}
	return s;
}