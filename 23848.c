void html_option(const char *value, const char *text, const char *selected_value)
{
	html("<option value='");
	html_attr(value);
	html("'");
	if (selected_value && !strcmp(selected_value, value))
		html(" selected='selected'");
	html(">");
	html_txt(text);
	html("</option>\n");
}