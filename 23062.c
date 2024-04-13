const char *qh_strerror(int code)
{
	if (code < 0) {
		return "Low-level system error";
	}

	if (code == 100) {
		return "Continue";
	}
	if (code == 101) {
		return "Switching protocols";
	}

	if (code < 300) {
		return "OK";
	}

	if (code < 400) {
		return "Redirected (possibly deprecated address)";
	}

	switch (code) {

	/* client errors */
	case 400: return "Bad request";
	case 401: return "Unauthorized";
	case 403: return "Forbidden (disabled by config)";
	case 404: return "Not found";
	case 405: return "Method not allowed";
	case 406: return "Not acceptable";
	case 407: return "Proxy authentication required";
	case 408: return "Request timed out";
	case 409: return "Conflict";
	case 410: return "Gone";
	case 411: return "Length required";
	case 412: return "Precondition failed";
	case 413: return "Request too large";
	case 414: return "Request-URI too long";

	/* server errors */
	case 500: return "Internal server error";
	case 501: return "Not implemented";
	case 502: return "Bad gateway";
	case 503: return "Service unavailable";
	case 504: return "Gateway timeout";
	case 505: return "Version not supported";

	}

	return "Unknown error";
}