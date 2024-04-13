void smtp_command_parser_deinit(struct smtp_command_parser **_parser)
{
	struct smtp_command_parser *parser = *_parser;

	i_stream_unref(&parser->data);
	i_free(parser->state.cmd_name);
	i_free(parser->state.cmd_params);
	i_free(parser->error);
	i_stream_unref(&parser->input);
	i_free(parser);
	*_parser = NULL;
}