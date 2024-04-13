check_websafe_message_contents(struct sshkey *key, struct sshbuf *data)
{
	if (parse_userauth_request(data, key, NULL, NULL) == 0) {
		debug_f("signed data matches public key userauth request");
		return 1;
	}
	if (parse_sshsig_request(data) == 0) {
		debug_f("signed data matches SSHSIG signature request");
		return 1;
	}

	/* XXX check CA signature operation */

	error("web-origin key attempting to sign non-SSH message");
	return 0;
}