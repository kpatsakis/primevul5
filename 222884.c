parse_bool(const char *line, int varlen, int *flags, int fval)
{
    debug_decl(parse_bool, SUDOERS_DEBUG_PLUGIN);

    switch (sudo_strtobool(line + varlen + 1)) {
    case true:
	SET(*flags, fval);
	debug_return_int(true);
    case false:
	CLR(*flags, fval);
	debug_return_int(false);
    default:
	sudo_warn(U_("invalid %.*s set by sudo front-end"),
	    varlen, line);
	debug_return_int(-1);
    }
}