static void print_session_info(const struct  get_session_info_rsp * session_info,
							   int data_len)
{
	if (csv_output)
		print_session_info_csv(session_info, data_len);
	else
		print_session_info_verbose(session_info, data_len);
}