static const char *assoc_failure_reason_to_str(u16 cap_info)
{
	switch (cap_info) {
	case CONNECT_ERR_AUTH_ERR_STA_FAILURE:
		return "CONNECT_ERR_AUTH_ERR_STA_FAILURE";
	case CONNECT_ERR_AUTH_MSG_UNHANDLED:
		return "CONNECT_ERR_AUTH_MSG_UNHANDLED";
	case CONNECT_ERR_ASSOC_ERR_TIMEOUT:
		return "CONNECT_ERR_ASSOC_ERR_TIMEOUT";
	case CONNECT_ERR_ASSOC_ERR_AUTH_REFUSED:
		return "CONNECT_ERR_ASSOC_ERR_AUTH_REFUSED";
	case CONNECT_ERR_STA_FAILURE:
		return "CONNECT_ERR_STA_FAILURE";
	}

	return "Unknown connect failure";
}