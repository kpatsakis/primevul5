lka_report_smtp_filter_response(const char *direction, struct timeval *tv, uint64_t reqid,
    int phase, int response, const char *param)
{
	const char *phase_name;
	const char *response_name;

	switch (phase) {
	case FILTER_CONNECT:
		phase_name = "connected";
		break;
	case FILTER_HELO:
		phase_name = "helo";
		break;
	case FILTER_EHLO:
		phase_name = "ehlo";
		break;
	case FILTER_STARTTLS:
		phase_name = "tls";
		break;
	case FILTER_AUTH:
		phase_name = "auth";
		break;
	case FILTER_MAIL_FROM:
		phase_name = "mail-from";
		break;
	case FILTER_RCPT_TO:
		phase_name = "rcpt-to";
		break;
	case FILTER_DATA:
		phase_name = "data";
		break;
	case FILTER_DATA_LINE:
		phase_name = "data-line";
		break;
	case FILTER_RSET:
		phase_name = "rset";
		break;
	case FILTER_QUIT:
		phase_name = "quit";
		break;
	case FILTER_NOOP:
		phase_name = "noop";
		break;
	case FILTER_HELP:
		phase_name = "help";
		break;
	case FILTER_WIZ:
		phase_name = "wiz";
		break;
	case FILTER_COMMIT:
		phase_name = "commit";
		break;
	default:
		phase_name = "";
	}

	switch (response) {
	case FILTER_PROCEED:
		response_name = "proceed";
		break;
	case FILTER_JUNK:
		response_name = "junk";
		break;
	case FILTER_REWRITE:
		response_name = "rewrite";
		break;
	case FILTER_REJECT:
		response_name = "reject";
		break;
	case FILTER_DISCONNECT:
		response_name = "disconnect";
		break;
	default:
		response_name = "";
	}

	report_smtp_broadcast(reqid, direction, tv, "filter-response",
	    "%s|%s%s%s\n", phase_name, response_name, param ? "|" : "",
	    param ? param : "");
}