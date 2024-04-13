void html_fileperm(unsigned short mode)
{
	htmlf("%c%c%c", (mode & 4 ? 'r' : '-'),
	      (mode & 2 ? 'w' : '-'), (mode & 1 ? 'x' : '-'));
}