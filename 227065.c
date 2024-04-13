process_sign_request2(SocketEntry *e)
{
	u_char *signature = NULL;
	size_t slen = 0;
	u_int compat = 0, flags;
	int r, ok = -1;
	char *fp = NULL;
	struct sshbuf *msg = NULL, *data = NULL;
	struct sshkey *key = NULL;
	struct identity *id;
	struct notifier_ctx *notifier = NULL;

	debug_f("entering");

	if ((msg = sshbuf_new()) == NULL || (data = sshbuf_new()) == NULL)
		fatal_f("sshbuf_new failed");
	if ((r = sshkey_froms(e->request, &key)) != 0 ||
	    (r = sshbuf_get_stringb(e->request, data)) != 0 ||
	    (r = sshbuf_get_u32(e->request, &flags)) != 0) {
		error_fr(r, "parse");
		goto send;
	}

	if ((id = lookup_identity(key)) == NULL) {
		verbose_f("%s key not found", sshkey_type(key));
		goto send;
	}
	if (id->confirm && confirm_key(id, NULL) != 0) {
		verbose_f("user refused key");
		goto send;
	}
	if (sshkey_is_sk(id->key)) {
		if (strncmp(id->key->sk_application, "ssh:", 4) != 0 &&
		    !check_websafe_message_contents(key, data)) {
			/* error already logged */
			goto send;
		}
		if ((id->key->sk_flags & SSH_SK_USER_PRESENCE_REQD)) {
			if ((fp = sshkey_fingerprint(key, SSH_FP_HASH_DEFAULT,
			    SSH_FP_DEFAULT)) == NULL)
				fatal_f("fingerprint failed");
			notifier = notify_start(0,
			    "Confirm user presence for key %s %s",
			    sshkey_type(id->key), fp);
		}
	}
	/* XXX support PIN required FIDO keys */
	if ((r = sshkey_sign(id->key, &signature, &slen,
	    sshbuf_ptr(data), sshbuf_len(data), agent_decode_alg(key, flags),
	    id->sk_provider, NULL, compat)) != 0) {
		error_fr(r, "sshkey_sign");
		goto send;
	}
	/* Success */
	ok = 0;
 send:
	notify_complete(notifier, "User presence confirmed");

	if (ok == 0) {
		if ((r = sshbuf_put_u8(msg, SSH2_AGENT_SIGN_RESPONSE)) != 0 ||
		    (r = sshbuf_put_string(msg, signature, slen)) != 0)
			fatal_fr(r, "compose");
	} else if ((r = sshbuf_put_u8(msg, SSH_AGENT_FAILURE)) != 0)
		fatal_fr(r, "compose failure");

	if ((r = sshbuf_put_stringb(e->output, msg)) != 0)
		fatal_fr(r, "enqueue");

	sshbuf_free(data);
	sshbuf_free(msg);
	sshkey_free(key);
	free(fp);
	free(signature);
}