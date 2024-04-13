static const char *prot_name(enum protocol protocol)
{
	switch (protocol) {
		case PROTO_LOCAL:
		case PROTO_FILE:
			return "file";
		case PROTO_SSH:
			return "ssh";
		case PROTO_GIT:
			return "git";
		default:
			return "unknown protocol";
	}
}