SMBC_call_auth_fn(TALLOC_CTX *ctx,
                  SMBCCTX *context,
                  const char *server,
                  const char *share,
                  char **pp_workgroup,
                  char **pp_username,
                  char **pp_password)
{
	fstring workgroup;
	fstring username;
	fstring password;
        smbc_get_auth_data_with_context_fn auth_with_context_fn;

	strlcpy(workgroup, *pp_workgroup, sizeof(workgroup));
	strlcpy(username, *pp_username, sizeof(username));
	strlcpy(password, *pp_password, sizeof(password));

        /* See if there's an authentication with context function provided */
        auth_with_context_fn = smbc_getFunctionAuthDataWithContext(context);
        if (auth_with_context_fn)
        {
            (* auth_with_context_fn)(context,
                                     server, share,
                                     workgroup, sizeof(workgroup),
                                     username, sizeof(username),
                                     password, sizeof(password));
        }
        else
        {
            smbc_getFunctionAuthData(context)(server, share,
                                              workgroup, sizeof(workgroup),
                                              username, sizeof(username),
                                              password, sizeof(password));
        }

	TALLOC_FREE(*pp_workgroup);
	TALLOC_FREE(*pp_username);
	TALLOC_FREE(*pp_password);

	*pp_workgroup = talloc_strdup(ctx, workgroup);
	*pp_username = talloc_strdup(ctx, username);
	*pp_password = talloc_strdup(ctx, password);
}
