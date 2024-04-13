smtp_command_parse_data_with_dot(struct smtp_command_parser *parser)
{
	struct istream *data;
	i_assert(parser->data == NULL);

	data = i_stream_create_dot(parser->input, TRUE);
	if (parser->limits.max_data_size != (uoff_t)-1) {
		parser->data = i_stream_create_failure_at(
			data, parser->limits.max_data_size, EMSGSIZE,
			t_strdup_printf("Command data size exceeds maximum "
					"(> %"PRIuUOFF_T")",
					parser->limits.max_data_size));
		i_stream_unref(&data);
	} else {
		parser->data = data;
	}
	i_stream_ref(parser->data);
	return parser->data;
}