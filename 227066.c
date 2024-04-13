process_add_identity(SocketEntry *e)
{
	Identity *id;
	int success = 0, confirm = 0;
	char *fp, *comment = NULL, *sk_provider = NULL;
	char canonical_provider[PATH_MAX];
	time_t death = 0;
	u_int seconds = 0;
	struct sshkey *k = NULL;
	int r = SSH_ERR_INTERNAL_ERROR;

	debug2_f("entering");
	if ((r = sshkey_private_deserialize(e->request, &k)) != 0 ||
	    k == NULL ||
	    (r = sshbuf_get_cstring(e->request, &comment, NULL)) != 0) {
		error_fr(r, "parse");
		goto out;
	}
	if (parse_key_constraints(e->request, k, &death, &seconds, &confirm,
	    &sk_provider) != 0) {
		error_f("failed to parse constraints");
		sshbuf_reset(e->request);
		goto out;
	}

	if (sk_provider != NULL) {
		if (!sshkey_is_sk(k)) {
			error("Cannot add provider: %s is not an "
			    "authenticator-hosted key", sshkey_type(k));
			goto out;
		}
		if (strcasecmp(sk_provider, "internal") == 0) {
			debug_f("internal provider");
		} else {
			if (realpath(sk_provider, canonical_provider) == NULL) {
				verbose("failed provider \"%.100s\": "
				    "realpath: %s", sk_provider,
				    strerror(errno));
				goto out;
			}
			free(sk_provider);
			sk_provider = xstrdup(canonical_provider);
			if (match_pattern_list(sk_provider,
			    allowed_providers, 0) != 1) {
				error("Refusing add key: "
				    "provider %s not allowed", sk_provider);
				goto out;
			}
		}
	}
	if ((r = sshkey_shield_private(k)) != 0) {
		error_fr(r, "shield private");
		goto out;
	}
	if (lifetime && !death)
		death = monotime() + lifetime;
	if ((id = lookup_identity(k)) == NULL) {
		id = xcalloc(1, sizeof(Identity));
		TAILQ_INSERT_TAIL(&idtab->idlist, id, next);
		/* Increment the number of identities. */
		idtab->nentries++;
	} else {
		/* key state might have been updated */
		sshkey_free(id->key);
		free(id->comment);
		free(id->sk_provider);
	}
	/* success */
	id->key = k;
	id->comment = comment;
	id->death = death;
	id->confirm = confirm;
	id->sk_provider = sk_provider;

	if ((fp = sshkey_fingerprint(k, SSH_FP_HASH_DEFAULT,
	    SSH_FP_DEFAULT)) == NULL)
		fatal_f("sshkey_fingerprint failed");
	debug_f("add %s %s \"%.100s\" (life: %u) (confirm: %u) "
	    "(provider: %s)", sshkey_ssh_name(k), fp, comment, seconds,
	    confirm, sk_provider == NULL ? "none" : sk_provider);
	free(fp);
	/* transferred */
	k = NULL;
	comment = NULL;
	sk_provider = NULL;
	success = 1;
 out:
	free(sk_provider);
	free(comment);
	sshkey_free(k);
	send_status(e, success);
}