static void create_refuse_error(int which)
{
	/* The "which" value is the index + OPT_REFUSED_BASE. */
	struct poptOption *op = &long_options[which - OPT_REFUSED_BASE];
	int n = snprintf(err_buf, sizeof err_buf,
			 "The server is configured to refuse --%s\n",
			 op->longName) - 1;
	if (op->shortName) {
		snprintf(err_buf + n, sizeof err_buf - n,
			 " (-%c)\n", op->shortName);
	}
}