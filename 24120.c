smtp_command_parser_restart(struct smtp_command_parser *parser)
{
	i_free(parser->state.cmd_name);
	i_free(parser->state.cmd_params);

	i_zero(&parser->state);
}