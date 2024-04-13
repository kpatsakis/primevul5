static int rtnl_dump_done(struct nlmsghdr *h)
{
	int len = *(int *)NLMSG_DATA(h);

	if (h->nlmsg_len < NLMSG_LENGTH(sizeof(int))) {
		fprintf(stderr, "DONE truncated\n");
		return -1;
	}

	if (len < 0) {
		errno = -len;
		switch (errno) {
		case ENOENT:
		case EOPNOTSUPP:
			return -1;
		case EMSGSIZE:
			fprintf(stderr,
				"Error: Buffer too small for object.\n");
			break;
		default:
			perror("RTNETLINK answers");
		}
		return len;
	}

	/* check for any messages returned from kernel */
	nl_dump_ext_ack(h, NULL);

	return 0;
}