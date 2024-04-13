int smtp_command_parse_auth_response(struct smtp_command_parser *parser,
			    const char **line_r,
			    enum smtp_command_parse_error *error_code_r,
			    const char **error_r)
{
	int ret;

	i_assert(parser->auth_response ||
		parser->state.state == SMTP_COMMAND_PARSE_STATE_INIT ||
		parser->state.state == SMTP_COMMAND_PARSE_STATE_ERROR);
	parser->auth_response = TRUE;

	*error_code_r = parser->error_code = SMTP_COMMAND_PARSE_ERROR_NONE;
	*error_r = NULL;

	i_free_and_null(parser->error);

	/* make sure we finished streaming payload from previous command
	   before we continue. */
	if ((ret = smtp_command_parse_finish_data(parser)) <= 0) {
		if (ret < 0) {
			*error_code_r = parser->error_code;
			*error_r = parser->error;
		}
		return ret;
	}

	if ((ret=smtp_command_parse(parser)) <= 0) {
		if (ret < 0) {
			*error_code_r = parser->error_code;
			*error_r = parser->error;
			parser->state.state = SMTP_COMMAND_PARSE_STATE_ERROR;
		}
		return ret;
	}

	i_assert(parser->state.state == SMTP_COMMAND_PARSE_STATE_INIT);
	*line_r = parser->state.cmd_params;
	parser->auth_response = FALSE;
	return 1;
}