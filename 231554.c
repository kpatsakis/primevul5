void trace_parser_put(struct trace_parser *parser)
{
	kfree(parser->buffer);
	parser->buffer = NULL;
}