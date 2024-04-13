void html_hidden(char *name, char *value)
{
	html("<input type='hidden' name='");
	html_attr(name);
	html("' value='");
	html_attr(value);
	html("'/>");
}