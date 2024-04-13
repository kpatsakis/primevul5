SMBC_remove_unused_server(SMBCCTX * context,
                          SMBCSRV * srv)
{
	SMBCFILE * file;

	/* are we being fooled ? */
	if (!context || !context->internal->initialized || !srv) {
                return 1;
        }

	/* Check all open files/directories for a relation with this server */
	for (file = context->internal->files; file; file = file->next) {
		if (file->srv == srv) {
			/* Still used */
			DEBUG(3, ("smbc_remove_usused_server: "
                                  "%p still used by %p.\n",
				  srv, file));
			return 1;
		}
	}

	DLIST_REMOVE(context->internal->servers, srv);

	cli_shutdown(srv->cli);
	srv->cli = NULL;

	DEBUG(3, ("smbc_remove_usused_server: %p removed.\n", srv));

	smbc_getFunctionRemoveCachedServer(context)(context, srv);

        SAFE_FREE(srv);
	return 0;
}
