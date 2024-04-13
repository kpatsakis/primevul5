agent_decode_alg(struct sshkey *key, u_int flags)
{
	if (key->type == KEY_RSA) {
		if (flags & SSH_AGENT_RSA_SHA2_256)
			return "rsa-sha2-256";
		else if (flags & SSH_AGENT_RSA_SHA2_512)
			return "rsa-sha2-512";
	} else if (key->type == KEY_RSA_CERT) {
		if (flags & SSH_AGENT_RSA_SHA2_256)
			return "rsa-sha2-256-cert-v01@openssh.com";
		else if (flags & SSH_AGENT_RSA_SHA2_512)
			return "rsa-sha2-512-cert-v01@openssh.com";
	}
	return NULL;
}