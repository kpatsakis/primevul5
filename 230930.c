static void cursor_report(struct vc_data *vc, struct tty_struct *tty)
{
	char buf[40];

	sprintf(buf, "\033[%d;%dR", vc->vc_y + (vc->vc_decom ? vc->vc_top + 1 : 1), vc->vc_x + 1);
	respond_string(buf, tty->port);
}