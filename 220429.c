static void tty_show_fdinfo(struct seq_file *m, struct file *file)
{
	struct tty_struct *tty = file_tty(file);

	if (tty && tty->ops && tty->ops->show_fdinfo)
		tty->ops->show_fdinfo(tty, m);
}