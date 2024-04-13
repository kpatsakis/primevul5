sudoers_policy_check(int argc, char * const argv[], char *env_add[],
    char **command_infop[], char **argv_out[], char **user_env_out[],
    const char **errstr)
{
    struct sudoers_exec_args exec_args;
    int ret;
    debug_decl(sudoers_policy_check, SUDOERS_DEBUG_PLUGIN);

    if (!ISSET(sudo_mode, MODE_EDIT))
	SET(sudo_mode, MODE_RUN);

    exec_args.argv = argv_out;
    exec_args.envp = user_env_out;
    exec_args.info = command_infop;

    ret = sudoers_policy_main(argc, argv, 0, env_add, false, &exec_args);
    if (ret == true && sudo_version >= SUDO_API_MKVERSION(1, 3)) {
	/* Unset close function if we don't need it to avoid extra process. */
	if (!def_log_input && !def_log_output && !def_use_pty &&
	    !sudo_auth_needs_end_session())
	    sudoers_policy.close = NULL;
    }

    /* The audit functions set audit_msg on failure. */
    if (ret != 1 && audit_msg != NULL) {
	if (sudo_version >= SUDO_API_MKVERSION(1, 15))
	    *errstr = audit_msg;
    }
    debug_return_int(ret);
}