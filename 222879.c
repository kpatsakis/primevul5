sudoers_policy_open(unsigned int version, sudo_conv_t conversation,
    sudo_printf_t plugin_printf, char * const settings[],
    char * const user_info[], char * const envp[], char * const args[],
    const char **errstr)
{
    struct sudo_conf_debug_file_list debug_files = TAILQ_HEAD_INITIALIZER(debug_files);
    struct sudoers_open_info info;
    const char *cp, *plugin_path = NULL;
    char * const *cur;
    int ret;
    debug_decl(sudoers_policy_open, SUDOERS_DEBUG_PLUGIN);

    sudo_version = version;
    sudo_conv = conversation;
    sudo_printf = plugin_printf;

    /* Plugin args are only specified for API version 1.2 and higher. */
    if (sudo_version < SUDO_API_MKVERSION(1, 2))
	args = NULL;

    /* Initialize the debug subsystem.  */
    for (cur = settings; (cp = *cur) != NULL; cur++) {
	if (strncmp(cp, "debug_flags=", sizeof("debug_flags=") - 1) == 0) {
	    cp += sizeof("debug_flags=") - 1;
	    if (!sudoers_debug_parse_flags(&debug_files, cp))
		debug_return_int(-1);
	    continue;
	}
	if (strncmp(cp, "plugin_path=", sizeof("plugin_path=") - 1) == 0) {
	    plugin_path = cp + sizeof("plugin_path=") - 1;
	    continue;
	}
    }
    if (!sudoers_debug_register(plugin_path, &debug_files))
	debug_return_int(-1);

    /* Call the sudoers init function. */
    info.settings = settings;
    info.user_info = user_info;
    info.plugin_args = args;
    ret = sudoers_init(&info, envp);

    /* The audit functions set audit_msg on failure. */
    if (ret != 1 && audit_msg != NULL) {
	if (sudo_version >= SUDO_API_MKVERSION(1, 15))
	    *errstr = audit_msg;
    }
    debug_return_int(ret);
}