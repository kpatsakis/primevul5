static void tty_del_file(struct file *file)
{
	struct tty_file_private *priv = file->private_data;
	struct tty_struct *tty = priv->tty;

	spin_lock(&tty->files_lock);
	list_del(&priv->list);
	spin_unlock(&tty->files_lock);
	tty_free_file(file);
}