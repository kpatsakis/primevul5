pause_command()
{
    int text = 0;
    double sleep_time;
    static char *buf = NULL;

    c_token++;

#ifdef USE_MOUSE
    paused_for_mouse = 0;
    if (equals(c_token, "mouse")) {
	sleep_time = -1;
	c_token++;

/*	EAM FIXME - This is not the correct test; what we really want */
/*	to know is whether or not the terminal supports mouse feedback */
/*	if (term_initialised) { */
	if (mouse_setting.on && term) {
	    struct udvt_entry *current;
	    int end_condition = 0;

	    while (!(END_OF_COMMAND)) {
		if (almost_equals(c_token, "key$press")) {
		    end_condition |= PAUSE_KEYSTROKE;
		    c_token++;
		} else if (equals(c_token, ",")) {
		    c_token++;
		} else if (equals(c_token, "any")) {
		    end_condition |= PAUSE_ANY;
		    c_token++;
		} else if (equals(c_token, "button1")) {
		    end_condition |= PAUSE_BUTTON1;
		    c_token++;
		} else if (equals(c_token, "button2")) {
		    end_condition |= PAUSE_BUTTON2;
		    c_token++;
		} else if (equals(c_token, "button3")) {
		    end_condition |= PAUSE_BUTTON3;
		    c_token++;
		} else if (equals(c_token, "close")) {
		    end_condition |= PAUSE_WINCLOSE;
		    c_token++;
		} else
		    break;
	    }

	    if (end_condition)
		paused_for_mouse = end_condition;
	    else
		paused_for_mouse = PAUSE_CLICK;

	    /* Set the pause mouse return codes to -1 */
	    current = add_udv_by_name("MOUSE_KEY");
	    Ginteger(&current->udv_value,-1);
	    current = add_udv_by_name("MOUSE_BUTTON");
	    Ginteger(&current->udv_value,-1);
	} else
	    int_warn(NO_CARET, "Mousing not active");
    } else
#endif
	sleep_time = real_expression();

    if (END_OF_COMMAND) {
	free(buf); /* remove the previous message */
	buf = gp_strdup("paused"); /* default message, used in Windows GUI pause dialog */
    } else {
	char *tmp = try_to_get_string();
	if (!tmp)
	    int_error(c_token, "expecting string");
	else {
#ifdef _WIN32
	    free(buf);
	    buf = tmp;
	    if (sleep_time >= 0) {
		fputs(buf, stderr);
	    }
#elif defined(OS2)
	    free(buf);
	    buf = tmp;
	    if (strcmp(term->name, "pm") != 0 || sleep_time >= 0)
		fputs(buf, stderr);
#else /* Not _WIN32 or OS2 */
	    free(buf);
	    buf = tmp;
	    fputs(buf, stderr);
#endif
	    text = 1;
	}
    }

    if (sleep_time < 0) {
#if defined(_WIN32)
	ctrlc_flag = FALSE;
# if defined(WGP_CONSOLE) && defined(USE_MOUSE)
	if (!paused_for_mouse || !MousableWindowOpened()) {
	    int junk = 0;
	    if (buf) {
		/* Use of fprintf() triggers a bug in MinGW + SJIS encoding */
		fputs(buf, stderr); fputs("\n", stderr);
	    }
	    /* cannot use EAT_INPUT_WITH here */
	    do {
		junk = getch();
		if (ctrlc_flag)
		    bail_to_command_line();
	    } while (junk != EOF && junk != '\n' && junk != '\r');
	} else /* paused_for_mouse */
# endif /* !WGP_CONSOLE */
	{
	    if (!Pause(buf)) /* returns false if Ctrl-C or Cancel was pressed */
		bail_to_command_line();
	}
#elif defined(OS2) && defined(USE_MOUSE)
	if (strcmp(term->name, "pm") == 0) {
	    int rc;
	    if ((rc = PM_pause(buf)) == 0) {
		/* if (!CallFromRexx)
		 * would help to stop REXX programs w/o raising an error message
		 * in RexxInterface() ...
		 */
		bail_to_command_line();
	    } else if (rc == 2) {
		fputs(buf, stderr);
		text = 1;
		EAT_INPUT_WITH(fgetc(stdin));
	    }
	} else {
	    EAT_INPUT_WITH(fgetc(stdin));
	}
#else /* !(_WIN32 || OS2) */
# ifdef USE_MOUSE
	if (term && term->waitforinput) {
	    /* It does _not_ work to do EAT_INPUT_WITH(term->waitforinput()) */
	    term->waitforinput(0);
	} else
# endif /* USE_MOUSE */
# ifdef MSDOS
	{
	    int junk;
	    /* cannot use EAT_INPUT_WITH here */
	    do {
#  ifdef __DJGPP__
		/* We use getkey() since with DJGPP 2.05 and gcc 7.2,
		   getchar() requires two keystrokes. */
		junk = getkey();
#  else
		junk = getch();
#  endif
		/* Check if Ctrl-C was pressed */
		if (junk == 0x03)
		    bail_to_command_line();
	    } while (junk != EOF && junk != '\n' && junk != '\r');
	    fputc('\n', stderr);
	}
# else
	    EAT_INPUT_WITH(fgetc(stdin));
# endif

#endif /* !(_WIN32 || OS2) */
    }
    if (sleep_time > 0)
	timed_pause(sleep_time);

    if (text != 0 && sleep_time >= 0)
	fputc('\n', stderr);
    screen_ok = FALSE;

}