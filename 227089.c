process_remove_identity(SocketEntry *e)
{
	int r, success = 0;
	struct sshkey *key = NULL;
	Identity *id;

	debug2_f("entering");
	if ((r = sshkey_froms(e->request, &key)) != 0) {
		error_fr(r, "parse key");
		goto done;
	}
	if ((id = lookup_identity(key)) == NULL) {
		debug_f("key not found");
		goto done;
	}
	/* We have this key, free it. */
	if (idtab->nentries < 1)
		fatal_f("internal error: nentries %d", idtab->nentries);
	TAILQ_REMOVE(&idtab->idlist, id, next);
	free_identity(id);
	idtab->nentries--;
	success = 1;
 done:
	sshkey_free(key);
	send_status(e, success);
}