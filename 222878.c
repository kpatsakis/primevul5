sudoers_policy_init_session(struct passwd *pwd, char **user_env[],
    const char **errstr)
{
    int ret;
    debug_decl(sudoers_policy_init_session, SUDOERS_DEBUG_PLUGIN);

    /* user_env is only specified for API version 1.2 and higher. */
    if (sudo_version < SUDO_API_MKVERSION(1, 2))
	user_env = NULL;

    ret = sudo_auth_begin_session(pwd, user_env);

    if (ret == 1) {
	session_opened = true;
    } else if (audit_msg != NULL) {
	/* The audit functions set audit_msg on failure. */
	if (sudo_version >= SUDO_API_MKVERSION(1, 15))
	    *errstr = audit_msg;
    }
    debug_return_int(ret);
}