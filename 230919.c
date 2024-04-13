void putconsxy(struct vc_data *vc, unsigned char *p)
{
	hide_cursor(vc);
	gotoxy(vc, p[0], p[1]);
	set_cursor(vc);
}