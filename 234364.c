read_line(const char *prompt, int start)
{
    int more;
    char expand_prompt[40];

    current_prompt = prompt;	/* HBB NEW 20040727 */

    prompt_desc.dsc$w_length = strlen(prompt);
    prompt_desc.dsc$a_pointer = (char *) prompt;
    strcpy(expand_prompt, "_");
    strncat(expand_prompt, prompt, sizeof(expand_prompt) - 2);
    do {
	line_desc.dsc$w_length = MAX_LINE_LEN - start;
	line_desc.dsc$a_pointer = &gp_input_line[start];
	switch (status[1] = smg$read_composed_line(&vms_vkid, &vms_ktid, &line_desc, &prompt_desc, &vms_len)) {
	case SMG$_EOF:
	    done(EXIT_SUCCESS);	/* ^Z isn't really an error */
	    break;
	case RMS$_TNS:		/* didn't press return in time */
	    vms_len--;		/* skip the last character */
	    break;		/* and parse anyway */
	case RMS$_BES:		/* Bad Escape Sequence */
	case RMS$_PES:		/* Partial Escape Sequence */
	    sys$putmsg(status);
	    vms_len = 0;	/* ignore the line */
	    break;
	case SS$_NORMAL:
	    break;		/* everything's fine */
	default:
	    done(status[1]);	/* give the error message */
	}
	start += vms_len;
	gp_input_line[start] = NUL;
	inline_num++;
	if (gp_input_line[start - 1] == '\\') {
	    /* Allow for a continuation line. */
	    prompt_desc.dsc$w_length = strlen(expand_prompt);
	    prompt_desc.dsc$a_pointer = expand_prompt;
	    more = 1;
	    --start;
	} else {
	    line_desc.dsc$w_length = strlen(gp_input_line);
	    line_desc.dsc$a_pointer = gp_input_line;
	    more = 0;
	}
    } while (more);
    return 0;
}