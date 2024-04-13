srs_strerror(int code)
{
	switch (code) {
		/* Simple errors */
		case SRS_SUCCESS:
			return "Success";
		case SRS_ENOTSRSADDRESS:
			return "Not an SRS address.";

		/* Config errors */
		case SRS_ENOSECRETS:
			return "No secrets in SRS configuration.";
		case SRS_ESEPARATORINVALID:
			return "Invalid separator suggested.";

		/* Input errors */
		case SRS_ENOSENDERATSIGN:
			return "No at sign in sender address";
		case SRS_EBUFTOOSMALL:
			return "Buffer too small.";

		/* Syntax errors */
		case SRS_ENOSRS0HOST:
			return "No host in SRS0 address.";
		case SRS_ENOSRS0USER:
			return "No user in SRS0 address.";
		case SRS_ENOSRS0HASH:
			return "No hash in SRS0 address.";
		case SRS_ENOSRS0STAMP:
			return "No timestamp in SRS0 address.";
		case SRS_ENOSRS1HOST:
			return "No host in SRS1 address.";
		case SRS_ENOSRS1USER:
			return "No user in SRS1 address.";
		case SRS_ENOSRS1HASH:
			return "No hash in SRS1 address.";
		case SRS_EBADTIMESTAMPCHAR:
			return "Bad base32 character in timestamp.";
		case SRS_EHASHTOOSHORT:
			return "Hash too short in SRS address.";

		/* SRS errors */
		case SRS_ETIMESTAMPOUTOFDATE:
			return "Time stamp out of date.";
		case SRS_EHASHINVALID:
			return "Hash invalid in SRS address.";

		default:
			return "Unknown error in SRS library.";
	}
}