ex_colorscheme(exarg_T *eap)
{
    if (*eap->arg == NUL)
    {
#ifdef FEAT_EVAL
	char_u *expr = vim_strsave((char_u *)"g:colors_name");
	char_u *p = NULL;

	if (expr != NULL)
	{
	    ++emsg_off;
	    p = eval_to_string(expr, FALSE);
	    --emsg_off;
	    vim_free(expr);
	}
	if (p != NULL)
	{
	    msg((char *)p);
	    vim_free(p);
	}
	else
	    msg("default");
#else
	msg(_("unknown"));
#endif
    }
    else if (load_colors(eap->arg) == FAIL)
	semsg(_("E185: Cannot find color scheme '%s'"), eap->arg);

#ifdef FEAT_VTP
    else if (has_vtp_working())
    {
	// background color change requires clear + redraw
	update_screen(CLEAR);
	redrawcmd();
    }
#endif
}