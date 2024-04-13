sudoers_policy_store_result(bool accepted, char *argv[], char *envp[],
    mode_t cmnd_umask, char *iolog_path, void *v)
{
    struct sudoers_exec_args *exec_args = v;
    char **command_info;
    int info_len = 0;
    debug_decl(sudoers_policy_store_result, SUDOERS_DEBUG_PLUGIN);

    if (exec_args == NULL)
	debug_return_bool(true);	/* nothing to do */

    /* Increase the length of command_info as needed, it is *not* checked. */
    command_info = calloc(55, sizeof(char *));
    if (command_info == NULL)
	goto oom;

    if (safe_cmnd != NULL) {
	command_info[info_len] = sudo_new_key_val("command", safe_cmnd);
	if (command_info[info_len++] == NULL)
	    goto oom;
    }
    if (def_log_input || def_log_output) {
	if (iolog_path)
	    command_info[info_len++] = iolog_path;	/* now owned */
	if (def_log_input) {
	    if ((command_info[info_len++] = strdup("iolog_stdin=true")) == NULL)
		goto oom;
	    if ((command_info[info_len++] = strdup("iolog_ttyin=true")) == NULL)
		goto oom;
	}
	if (def_log_output) {
	    if ((command_info[info_len++] = strdup("iolog_stdout=true")) == NULL)
		goto oom;
	    if ((command_info[info_len++] = strdup("iolog_stderr=true")) == NULL)
		goto oom;
	    if ((command_info[info_len++] = strdup("iolog_ttyout=true")) == NULL)
		goto oom;
	}
	if (def_compress_io) {
	    if ((command_info[info_len++] = strdup("iolog_compress=true")) == NULL)
		goto oom;
	}
	if (def_iolog_flush) {
	    if ((command_info[info_len++] = strdup("iolog_flush=true")) == NULL)
		goto oom;
	}
	if (def_maxseq != NULL) {
	    if (asprintf(&command_info[info_len++], "maxseq=%s", def_maxseq) == -1)
		goto oom;
	}
    }
    if (ISSET(sudo_mode, MODE_EDIT)) {
	if ((command_info[info_len++] = strdup("sudoedit=true")) == NULL)
	    goto oom;
	if (!def_sudoedit_checkdir) {
	    if ((command_info[info_len++] = strdup("sudoedit_checkdir=false")) == NULL)
		goto oom;
	}
	if (def_sudoedit_follow) {
	    if ((command_info[info_len++] = strdup("sudoedit_follow=true")) == NULL)
		goto oom;
	}
    }
    if (def_runcwd && strcmp(def_runcwd, "*") != 0) {
	/* Set cwd to explicit value in sudoers. */
	if (!expand_tilde(&def_runcwd, runas_pw->pw_name)) {
	    sudo_warnx(U_("invalid working directory: %s"), def_runcwd);
	    goto bad;
	}
	if ((command_info[info_len++] = sudo_new_key_val("cwd", def_runcwd)) == NULL)
	    goto oom;
    } else if (ISSET(sudo_mode, MODE_LOGIN_SHELL)) {
	/* Set cwd to run user's homedir. */
	if ((command_info[info_len++] = sudo_new_key_val("cwd", runas_pw->pw_dir)) == NULL)
	    goto oom;
	if ((command_info[info_len++] = strdup("cwd_optional=true")) == NULL)
	    goto oom;
    }
    if ((command_info[info_len++] = sudo_new_key_val("runas_user", runas_pw->pw_name)) == NULL)
	goto oom;
    if (runas_gr != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("runas_group", runas_gr->gr_name)) == NULL)
	    goto oom;
    }
    if (def_stay_setuid) {
	if (asprintf(&command_info[info_len++], "runas_uid=%u",
	    (unsigned int)user_uid) == -1)
	    goto oom;
	if (asprintf(&command_info[info_len++], "runas_gid=%u",
	    (unsigned int)user_gid) == -1)
	    goto oom;
	if (asprintf(&command_info[info_len++], "runas_euid=%u",
	    (unsigned int)runas_pw->pw_uid) == -1)
	    goto oom;
	if (asprintf(&command_info[info_len++], "runas_egid=%u",
	    runas_gr ? (unsigned int)runas_gr->gr_gid :
	    (unsigned int)runas_pw->pw_gid) == -1)
	    goto oom;
    } else {
	if (asprintf(&command_info[info_len++], "runas_uid=%u",
	    (unsigned int)runas_pw->pw_uid) == -1)
	    goto oom;
	if (asprintf(&command_info[info_len++], "runas_gid=%u",
	    runas_gr ? (unsigned int)runas_gr->gr_gid :
	    (unsigned int)runas_pw->pw_gid) == -1)
	    goto oom;
    }
    if (def_preserve_groups) {
	if ((command_info[info_len++] = strdup("preserve_groups=true")) == NULL)
	    goto oom;
    } else {
	int i, len;
	gid_t egid;
	size_t glsize;
	char *cp, *gid_list;
	struct gid_list *gidlist;

	/* Only use results from a group db query, not the front end. */
	gidlist = sudo_get_gidlist(runas_pw, ENTRY_TYPE_QUERIED);

	/* We reserve an extra spot in the list for the effective gid. */
	glsize = sizeof("runas_groups=") - 1 +
	    ((gidlist->ngids + 1) * (MAX_UID_T_LEN + 1));
	gid_list = malloc(glsize);
	if (gid_list == NULL)
	    goto oom;
	memcpy(gid_list, "runas_groups=", sizeof("runas_groups=") - 1);
	cp = gid_list + sizeof("runas_groups=") - 1;

	/* On BSD systems the effective gid is the first group in the list. */
	egid = runas_gr ? (unsigned int)runas_gr->gr_gid :
	    (unsigned int)runas_pw->pw_gid;
	len = snprintf(cp, glsize - (cp - gid_list), "%u", (unsigned int)egid);
	if (len < 0 || (size_t)len >= glsize - (cp - gid_list)) {
	    sudo_warnx(U_("internal error, %s overflow"), __func__);
	    free(gid_list);
	    goto bad;
	}
	cp += len;
	for (i = 0; i < gidlist->ngids; i++) {
	    if (gidlist->gids[i] != egid) {
		len = snprintf(cp, glsize - (cp - gid_list), ",%u",
		     (unsigned int) gidlist->gids[i]);
		if (len < 0 || (size_t)len >= glsize - (cp - gid_list)) {
		    sudo_warnx(U_("internal error, %s overflow"), __func__);
		    free(gid_list);
		    goto bad;
		}
		cp += len;
	    }
	}
	command_info[info_len++] = gid_list;
	sudo_gidlist_delref(gidlist);
    }
    if (def_closefrom >= 0) {
	if (asprintf(&command_info[info_len++], "closefrom=%d", def_closefrom) == -1)
	    goto oom;
    }
    if (def_ignore_iolog_errors) {
	if ((command_info[info_len++] = strdup("ignore_iolog_errors=true")) == NULL)
	    goto oom;
    }
    if (def_noexec) {
	if ((command_info[info_len++] = strdup("noexec=true")) == NULL)
	    goto oom;
    }
    if (def_exec_background) {
	if ((command_info[info_len++] = strdup("exec_background=true")) == NULL)
	    goto oom;
    }
    if (def_set_utmp) {
	if ((command_info[info_len++] = strdup("set_utmp=true")) == NULL)
	    goto oom;
    }
    if (def_use_pty) {
	if ((command_info[info_len++] = strdup("use_pty=true")) == NULL)
	    goto oom;
    }
    if (def_utmp_runas) {
	if ((command_info[info_len++] = sudo_new_key_val("utmp_user", runas_pw->pw_name)) == NULL)
	    goto oom;
    }
    if (def_iolog_mode != (S_IRUSR|S_IWUSR)) {
	if (asprintf(&command_info[info_len++], "iolog_mode=0%o", (unsigned int)def_iolog_mode) == -1)
	    goto oom;
    }
    if (def_iolog_user != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("iolog_user", def_iolog_user)) == NULL)
	    goto oom;
    }
    if (def_iolog_group != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("iolog_group", def_iolog_group)) == NULL)
	    goto oom;
    }
    if (!SLIST_EMPTY(&def_log_servers)) {
	char *log_servers = serialize_list("log_servers", &def_log_servers);
	if (log_servers == NULL)
	    goto oom;
	command_info[info_len++] = log_servers;

	if (asprintf(&command_info[info_len++], "log_server_timeout=%u", def_log_server_timeout) == -1)
	    goto oom;
    }

    if ((command_info[info_len++] = sudo_new_key_val("log_server_keepalive",
	    def_log_server_keepalive ? "true" : "false")) == NULL)
        goto oom;

    if ((command_info[info_len++] = sudo_new_key_val("log_server_verify",
	    def_log_server_verify ? "true" : "false")) == NULL)
        goto oom;

    if (def_log_server_cabundle != NULL) {
        if ((command_info[info_len++] = sudo_new_key_val("log_server_cabundle", def_log_server_cabundle)) == NULL)
            goto oom;
    }
    if (def_log_server_peer_cert != NULL) {
        if ((command_info[info_len++] = sudo_new_key_val("log_server_peer_cert", def_log_server_peer_cert)) == NULL)
            goto oom;
    }
    if (def_log_server_peer_key != NULL) {
        if ((command_info[info_len++] = sudo_new_key_val("log_server_peer_key", def_log_server_peer_key)) == NULL)
            goto oom;
    }

    if (def_command_timeout > 0 || user_timeout > 0) {
	int timeout = user_timeout;
	if (timeout == 0 || def_command_timeout < timeout)
	    timeout = def_command_timeout;
	if (asprintf(&command_info[info_len++], "timeout=%u", timeout) == -1)
	    goto oom;
    }
    if (def_runchroot != NULL && strcmp(def_runchroot, "*") != 0) {
	if (!expand_tilde(&def_runchroot, runas_pw->pw_name)) {
	    sudo_warnx(U_("invalid chroot directory: %s"), def_runchroot);
	    goto bad;
	}
        if ((command_info[info_len++] = sudo_new_key_val("chroot", def_runchroot)) == NULL)
            goto oom;
    }
    if (cmnd_umask != ACCESSPERMS) {
	if (asprintf(&command_info[info_len++], "umask=0%o", (unsigned int)cmnd_umask) == -1)
	    goto oom;
    }
    if (force_umask) {
	if ((command_info[info_len++] = strdup("umask_override=true")) == NULL)
	    goto oom;
    }
    if (cmnd_fd != -1) {
	if (sudo_version < SUDO_API_MKVERSION(1, 9)) {
	    /* execfd only supported by plugin API 1.9 and higher */
	    close(cmnd_fd);
	    cmnd_fd = -1;
	} else {
	    if (asprintf(&command_info[info_len++], "execfd=%d", cmnd_fd) == -1)
		goto oom;
	}
    }
#ifdef HAVE_LOGIN_CAP_H
    if (def_use_loginclass) {
	if ((command_info[info_len++] = sudo_new_key_val("login_class", login_class)) == NULL)
	    goto oom;
    }
#endif /* HAVE_LOGIN_CAP_H */
#ifdef HAVE_SELINUX
    if (def_selinux && user_role != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("selinux_role", user_role)) == NULL)
	    goto oom;
    }
    if (def_selinux && user_type != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("selinux_type", user_type)) == NULL)
	    goto oom;
    }
#endif /* HAVE_SELINUX */
#ifdef HAVE_PRIV_SET
    if (runas_privs != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("runas_privs", runas_privs)) == NULL)
	    goto oom;
    }
    if (runas_limitprivs != NULL) {
	if ((command_info[info_len++] = sudo_new_key_val("runas_limitprivs", runas_limitprivs)) == NULL)
	    goto oom;
    }
#endif /* HAVE_SELINUX */

    /* Free on exit; they are not available in the close function. */
    sudoers_gc_add(GC_VECTOR, envp);
    sudoers_gc_add(GC_VECTOR, command_info);

    /* Fill in exec environment info. */
    *(exec_args->argv) = argv;
    *(exec_args->envp) = envp;
    *(exec_args->info) = command_info;

    debug_return_bool(true);

oom:
    sudo_warnx(U_("%s: %s"), __func__, U_("unable to allocate memory"));
bad:
    free(audit_msg);
    audit_msg = NULL;
    while (info_len--)
	free(command_info[info_len]);
    free(command_info);
    debug_return_bool(false);
}