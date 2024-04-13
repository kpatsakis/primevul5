sudoers_policy_list(int argc, char * const argv[], int verbose,
    const char *list_user, const char **errstr)
{
    int ret;
    debug_decl(sudoers_policy_list, SUDOERS_DEBUG_PLUGIN);

    user_cmnd = "list";
    if (argc)
	SET(sudo_mode, MODE_CHECK);
    else
	SET(sudo_mode, MODE_LIST);
    if (list_user) {
	list_pw = sudo_getpwnam(list_user);
	if (list_pw == NULL) {
	    sudo_warnx(U_("unknown user: %s"), list_user);
	    debug_return_int(-1);
	}
    }
    ret = sudoers_policy_main(argc, argv, I_LISTPW, NULL, verbose, NULL);
    if (list_user) {
	sudo_pw_delref(list_pw);
	list_pw = NULL;
    }

    /* The audit functions set audit_msg on failure. */
    if (ret != 1 && audit_msg != NULL) {
	if (sudo_version >= SUDO_API_MKVERSION(1, 15))
	    *errstr = audit_msg;
    }
    debug_return_int(ret);
}