process_add_smartcard_key(SocketEntry *e)
{
	char *provider = NULL, *pin = NULL, canonical_provider[PATH_MAX];
	char **comments = NULL;
	int r, i, count = 0, success = 0, confirm = 0;
	u_int seconds = 0;
	time_t death = 0;
	struct sshkey **keys = NULL, *k;
	Identity *id;

	debug2_f("entering");
	if ((r = sshbuf_get_cstring(e->request, &provider, NULL)) != 0 ||
	    (r = sshbuf_get_cstring(e->request, &pin, NULL)) != 0) {
		error_fr(r, "parse");
		goto send;
	}
	if (parse_key_constraints(e->request, NULL, &death, &seconds, &confirm,
	    NULL) != 0) {
		error_f("failed to parse constraints");
		goto send;
	}
	if (realpath(provider, canonical_provider) == NULL) {
		verbose("failed PKCS#11 add of \"%.100s\": realpath: %s",
		    provider, strerror(errno));
		goto send;
	}
	if (match_pattern_list(canonical_provider, allowed_providers, 0) != 1) {
		verbose("refusing PKCS#11 add of \"%.100s\": "
		    "provider not allowed", canonical_provider);
		goto send;
	}
	debug_f("add %.100s", canonical_provider);
	if (lifetime && !death)
		death = monotime() + lifetime;

	count = pkcs11_add_provider(canonical_provider, pin, &keys, &comments);
	for (i = 0; i < count; i++) {
		k = keys[i];
		if (lookup_identity(k) == NULL) {
			id = xcalloc(1, sizeof(Identity));
			id->key = k;
			keys[i] = NULL; /* transferred */
			id->provider = xstrdup(canonical_provider);
			if (*comments[i] != '\0') {
				id->comment = comments[i];
				comments[i] = NULL; /* transferred */
			} else {
				id->comment = xstrdup(canonical_provider);
			}
			id->death = death;
			id->confirm = confirm;
			TAILQ_INSERT_TAIL(&idtab->idlist, id, next);
			idtab->nentries++;
			success = 1;
		}
		/* XXX update constraints for existing keys */
		sshkey_free(keys[i]);
		free(comments[i]);
	}
send:
	free(pin);
	free(provider);
	free(keys);
	free(comments);
	send_status(e, success);
}