void html(const char *txt)
{
	write(htmlfd, txt, strlen(txt));
}