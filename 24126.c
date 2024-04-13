static int smtp_command_parse_identifier(struct smtp_command_parser *parser)
{
	const unsigned char *p;

	/* The commands themselves are alphabetic characters.
	 */
	p = parser->cur + parser->state.poff;
	i_assert(p <= parser->end);
	while (p < parser->end && i_isalpha(*p))
		p++;
	if ((p - parser->cur) > SMTP_COMMAND_PARSER_MAX_COMMAND_LENGTH) {
		smtp_command_parser_error(parser,
			SMTP_COMMAND_PARSE_ERROR_BAD_COMMAND,
			"Command name is too long");
		return -1;
	}
	parser->state.poff = p - parser->cur;
	if (p == parser->end)
		return 0;
	parser->state.cmd_name = str_ucase(i_strdup_until(parser->cur, p));
	parser->cur = p;
	parser->state.poff = 0;
	return 1;
}