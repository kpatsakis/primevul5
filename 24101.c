auth_request_str_escape(const char *string,
			const struct auth_request *request ATTR_UNUSED)
{
	return str_escape(string);
}