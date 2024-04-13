smtp_command_parse_line(struct smtp_command_parser *parser)
{
	int ret;

	/* RFC 5321, Section 4.1.1:

	   SMTP commands are character strings terminated by <CRLF>. The
	   commands themselves are alphabetic characters terminated by <SP> if
	   parameters follow and <CRLF> otherwise. (In the interest of improved
	   interoperability, SMTP receivers SHOULD tolerate trailing white space
	   before the terminating <CRLF>.)
	 */
	for (;;) {
		switch (parser->state.state) {
		case SMTP_COMMAND_PARSE_STATE_INIT:
			smtp_command_parser_restart(parser);
			if (parser->auth_response) {
				/* parse AUTH response as bare parameters */
				parser->state.state =
					SMTP_COMMAND_PARSE_STATE_PARAMETERS;
			} else {
				parser->state.state =
					SMTP_COMMAND_PARSE_STATE_COMMAND;
			}
			if (parser->cur == parser->end)
				return 0;
			if (parser->auth_response)
				break;
			/* fall through */
		case SMTP_COMMAND_PARSE_STATE_COMMAND:
			if ((ret=smtp_command_parse_identifier(parser)) <= 0)
				return ret;
			parser->state.state = SMTP_COMMAND_PARSE_STATE_SP;
			if (parser->cur == parser->end)
				return 0;
			/* fall through */
		case SMTP_COMMAND_PARSE_STATE_SP:
			if (*parser->cur == '\r') {
				parser->state.state =
					SMTP_COMMAND_PARSE_STATE_CR;
				break;
			} else if (*parser->cur == '\n') {
				parser->state.state =
					SMTP_COMMAND_PARSE_STATE_LF;
				break;
			} else if (*parser->cur != ' ') {
				smtp_command_parser_error(parser,
					SMTP_COMMAND_PARSE_ERROR_BAD_COMMAND,
					"Unexpected character %s in command name",
					_chr_sanitize(*parser->cur));
				return -1;
			}
			parser->cur++;
			parser->state.state =
				SMTP_COMMAND_PARSE_STATE_PARAMETERS;
			if (parser->cur >= parser->end)
				return 0;
			/* fall through */
		case SMTP_COMMAND_PARSE_STATE_PARAMETERS:
			if ((ret=smtp_command_parse_parameters(parser)) <= 0)
				return ret;
			parser->state.state = SMTP_COMMAND_PARSE_STATE_CR;
			if (parser->cur == parser->end)
				return 0;
			/* fall through */
		case SMTP_COMMAND_PARSE_STATE_CR:
			if (*parser->cur == '\r') {
				parser->cur++;
			} else if (*parser->cur != '\n') {
				smtp_command_parser_error(parser,
					SMTP_COMMAND_PARSE_ERROR_BAD_COMMAND,
					"Unexpected character %s in %s",
					_chr_sanitize(*parser->cur),
					(parser->auth_response ?
						"AUTH response" :
						"command parameters"));
				return -1;
			}
			parser->state.state = SMTP_COMMAND_PARSE_STATE_LF;
			if (parser->cur == parser->end)
				return 0;
			/* fall through */
		case SMTP_COMMAND_PARSE_STATE_LF:
			if (*parser->cur != '\n') {
				smtp_command_parser_error(parser,
					SMTP_COMMAND_PARSE_ERROR_BAD_COMMAND,
					"Expected LF after CR at end of %s, "
					"but found %s",
					(parser->auth_response ?
						"AUTH response" : "command"),
					_chr_sanitize(*parser->cur));
				return -1;
			}
			parser->cur++;
			parser->state.state = SMTP_COMMAND_PARSE_STATE_INIT;
			return 1;
		case SMTP_COMMAND_PARSE_STATE_ERROR:
			/* skip until end of line */
			while (parser->cur < parser->end &&
			       *parser->cur != '\n')
				parser->cur++;
			if (parser->cur == parser->end)
				return 0;
			parser->cur++;
			parser->state.state = SMTP_COMMAND_PARSE_STATE_INIT;
			break;
		default:
			i_unreached();
		}
	}

	i_unreached();
	return -1;
}