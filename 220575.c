f_fullcommand(typval_T *argvars, typval_T *rettv)
{
    exarg_T  ea;
    char_u   *name;
    char_u   *p;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    if (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	return;

    name = argvars[0].vval.v_string;
    if (name == NULL)
	return;

    while (*name != NUL && *name == ':')
	name++;
    name = skip_range(name, TRUE, NULL);

    ea.cmd = (*name == '2' || *name == '3') ? name + 1 : name;
    ea.cmdidx = (cmdidx_T)0;
    ea.addr_count = 0;
    p = find_ex_command(&ea, NULL, NULL, NULL);
    if (p == NULL || ea.cmdidx == CMD_SIZE)
	return;
    if (in_vim9script())
    {
	int	     res;

	++emsg_silent;
	res = not_in_vim9(&ea);
	--emsg_silent;

	if (res == FAIL)
	    return;
    }

    rettv->vval.v_string = vim_strsave(IS_USER_CMDIDX(ea.cmdidx)
				 ? get_user_command_name(ea.useridx, ea.cmdidx)
				 : cmdnames[ea.cmdidx].cmd_name);
}