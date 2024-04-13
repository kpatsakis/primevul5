process_remove_all_identities(SocketEntry *e)
{
	Identity *id;

	debug2_f("entering");
	/* Loop over all identities and clear the keys. */
	for (id = TAILQ_FIRST(&idtab->idlist); id;
	    id = TAILQ_FIRST(&idtab->idlist)) {
		TAILQ_REMOVE(&idtab->idlist, id, next);
		free_identity(id);
	}

	/* Mark that there are no identities. */
	idtab->nentries = 0;

	/* Send success. */
	send_status(e, 1);
}