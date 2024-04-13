init_terminal()
{
    char *term_name = DEFAULTTERM;
#if defined(__BEOS__) || defined(X11)
    char *env_term = NULL;      /* from TERM environment var */
#endif
#ifdef X11
    char *display = NULL;
#endif
    char *gnuterm = NULL;

    /* GNUTERM environment variable is primary */
    gnuterm = getenv("GNUTERM");
    if (gnuterm != (char *) NULL) {
	/* April 2017 - allow GNUTERM to include terminal options */
	char *set_term = "set term ";
	char *set_term_command = gp_alloc(strlen(set_term) + strlen(gnuterm) + 4, NULL);
	strcpy(set_term_command, set_term);
	strcat(set_term_command, gnuterm);
	do_string(set_term_command);
	free(set_term_command);
	/* replicate environmental variable GNUTERM for internal use */
	Gstring(&(add_udv_by_name("GNUTERM")->udv_value), gp_strdup(gnuterm));
	return;

    } else {

#ifdef VMS
	term_name = vms_init();
#endif /* VMS */

	if (term_name == (char *) NULL
            && getenv ("DOMTERM") != NULL)
          term_name = "domterm";

#ifdef __BEOS__
	env_term = getenv("TERM");
	if (term_name == (char *) NULL
	    && env_term != (char *) NULL && strcmp(env_term, "beterm") == 0)
	    term_name = "be";
#endif /* BeOS */

#ifdef QTTERM
	if (term_name == (char *) NULL)
	    term_name = "qt";
#endif

#ifdef WXWIDGETS
	if (term_name == (char *) NULL)
	    term_name = "wxt";
#endif

#ifdef _WIN32
	if (term_name == (char *) NULL)
	    term_name = "win";
#endif /* _WIN32 */

#if defined(__APPLE__) && defined(__MACH__) && defined(HAVE_FRAMEWORK_AQUATERM)
	/* Mac OS X with AquaTerm installed */
	term_name = "aqua";
#endif

#ifdef X11
	env_term = getenv("TERM");      /* try $TERM */
	if (term_name == (char *) NULL
	    && env_term != (char *) NULL && strcmp(env_term, "xterm") == 0)
	    term_name = "x11";
	display = getenv("DISPLAY");
	if (term_name == (char *) NULL && display != (char *) NULL)
	    term_name = "x11";
	if (X11_Display)
	    term_name = "x11";
#endif /* x11 */

#ifdef DJGPP
	term_name = "svga";
#endif

#ifdef GRASS
	term_name = "grass";
#endif

#ifdef OS2
/* amai: Note that we do some checks above and now overwrite any
   results. Perhaps we may disable checks above!? */
#ifdef X11
/* WINDOWID is set in sessions like xterm, etc.
   DISPLAY is also mandatory. */
	env_term = getenv("WINDOWID");
	display  = getenv("DISPLAY");
	if ((env_term != (char *) NULL) && (display != (char *) NULL))
	    term_name = "x11";
	else
#endif          /* X11 */
	    term_name = "pm";
#endif /*OS2 */

    }

    /* We have a name, try to set term type */
    if (term_name != NULL && *term_name != '\0') {
	int namelength = strlen(term_name);
	struct udvt_entry *name = add_udv_by_name("GNUTERM");

	Gstring(&name->udv_value, gp_strdup(term_name));

	if (strchr(term_name,' '))
	    namelength = strchr(term_name,' ') - term_name;

	/* Force the terminal to initialize default fonts, etc.	This prevents */
	/* segfaults and other strangeness if you set GNUTERM to "post" or    */
	/* "png" for example. However, calling X11_options() is expensive due */
	/* to the fork+execute of gnuplot_x11 and x11 can tolerate not being  */
	/* initialized until later.                                           */
	/* Note that gp_input_line[] is blank at this point.	              */
	if (change_term(term_name, namelength)) {
	    if (strcmp(term->name,"x11"))
		term->options();
	    return;
	}
	fprintf(stderr, "Unknown or ambiguous terminal name '%s'\n", term_name);
    }
    change_term("unknown", 7);
}