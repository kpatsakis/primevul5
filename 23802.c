void html_intoption(int value, const char *text, int selected_value)
{
	htmlf("<option value='%d'%s>", value,
	      value == selected_value ? " selected='selected'" : "");
	html_txt(text);
	html("</option>");
}