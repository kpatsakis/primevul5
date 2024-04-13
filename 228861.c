filter_protocol(uint64_t reqid, enum filter_phase phase, const char *param)
{
	struct filter_session  *fs;
	uint64_t		token = 0;
	char		       *nparam = NULL;
	
	fs = tree_xget(&sessions, reqid);

	switch (phase) {
	case FILTER_HELO:
	case FILTER_EHLO:
		free(fs->helo);
		fs->helo = xstrdup(param);
		break;
	case FILTER_MAIL_FROM:
		free(fs->mail_from);
		fs->mail_from = xstrdup(param + 1);
		*strchr(fs->mail_from, '>') = '\0';
		param = fs->mail_from;

		break;
	case FILTER_RCPT_TO:
		nparam = xstrdup(param + 1);
		*strchr(nparam, '>') = '\0';
		param = nparam;
		break;
	case FILTER_STARTTLS:
		/* TBD */
		break;
	default:
		break;
	}

	free(fs->lastparam);
	fs->lastparam = xstrdup(param);

	filter_protocol_internal(fs, &token, reqid, phase, param);
	if (nparam)
		free(nparam);
}