void smtp_command_parser_set_stream(struct smtp_command_parser *parser,
	struct istream *input)
{
	i_stream_unref(&parser->input);
	if (input != NULL) {
		parser->input = input;
		i_stream_ref(parser->input);
	}
}