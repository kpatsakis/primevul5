usage(void)
{
	fprintf(stderr,
	    "usage: ssh-agent [-c | -s] [-Dd] [-a bind_address] [-E fingerprint_hash]\n"
	    "                 [-P allowed_providers] [-t life]\n"
	    "       ssh-agent [-a bind_address] [-E fingerprint_hash] [-P allowed_providers]\n"
	    "                 [-t life] command [arg ...]\n"
	    "       ssh-agent [-c | -s] -k\n");
	exit(1);
}