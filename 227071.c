process_request_identities(SocketEntry *e)
{
	Identity *id;
	struct sshbuf *msg;
	int r;

	debug2_f("entering");

	if ((msg = sshbuf_new()) == NULL)
		fatal_f("sshbuf_new failed");
	if ((r = sshbuf_put_u8(msg, SSH2_AGENT_IDENTITIES_ANSWER)) != 0 ||
	    (r = sshbuf_put_u32(msg, idtab->nentries)) != 0)
		fatal_fr(r, "compose");
	TAILQ_FOREACH(id, &idtab->idlist, next) {
		if ((r = sshkey_puts_opts(id->key, msg, SSHKEY_SERIALIZE_INFO))
		     != 0 ||
		    (r = sshbuf_put_cstring(msg, id->comment)) != 0) {
			error_fr(r, "compose key/comment");
			continue;
		}
	}
	if ((r = sshbuf_put_stringb(e->output, msg)) != 0)
		fatal_fr(r, "enqueue");
	sshbuf_free(msg);
}