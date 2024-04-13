expand_1level_macros()
{
    TBOOLEAN in_squote = FALSE;
    TBOOLEAN in_dquote = FALSE;
    TBOOLEAN after_backslash = FALSE;
    TBOOLEAN in_comment= FALSE;
    int   len;
    int   o = 0;
    int   nfound = 0;
    char *c;
    char *temp_string;
    char  temp_char;
    char *m;
    struct udvt_entry *udv;

    /* Most lines have no macros */
    if (!strchr(gp_input_line,'@'))
	return(0);

    temp_string = gp_alloc(gp_input_line_len,"string variable");
    len = strlen(gp_input_line);
    if (len >= gp_input_line_len) len = gp_input_line_len-1;
    strncpy(temp_string,gp_input_line,len);
    temp_string[len] = '\0';

    for (c=temp_string; len && c && *c; c++, len--) {
	switch (*c) {
	case '@':	/* The only tricky bit */
		if (!in_squote && !in_dquote && !in_comment && isalpha((unsigned char)c[1])) {
		    /* Isolate the udv key as a null-terminated substring */
		    m = ++c;
		    while (isalnum((unsigned char )*c) || (*c=='_')) c++;
		    temp_char = *c; *c = '\0';
		    /* Look up the key and restore the original following char */
		    udv = get_udv_by_name(m);
		    if (udv && udv->udv_value.type == STRING) {
			nfound++;
			m = udv->udv_value.v.string_val;
			FPRINTF((stderr,"Replacing @%s with \"%s\"\n",udv->udv_name,m));
			while (strlen(m) + o + len > gp_input_line_len)
			    extend_input_line();
			while (*m)
			    gp_input_line[o++] = (*m++);
		    } else {
			int_warn( NO_CARET, "%s is not a string variable",m);
		    }
		    *c-- = temp_char;
		} else
		    COPY_CHAR;
		break;

	case '"':
		if (!after_backslash)
		    in_dquote = !in_dquote;
		COPY_CHAR; break;
	case '\'':
		in_squote = !in_squote;
		COPY_CHAR; break;
	case '\\':
		if (in_dquote)
		    after_backslash = !after_backslash;
		gp_input_line[o++] = *c; break;
	case '#':
		if (!in_squote && !in_dquote)
		    in_comment = TRUE;
	default :
		COPY_CHAR; break;
	}
    }
    gp_input_line[o] = '\0';
    free(temp_string);

    if (nfound)
	FPRINTF((stderr,
		 "After string substitution command line is:\n\t%s\n",
		 gp_input_line));

    return(nfound);
}