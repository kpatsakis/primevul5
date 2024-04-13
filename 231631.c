void chrdev_show(struct seq_file *f, off_t offset)
{
	struct char_device_struct *cd;

	mutex_lock(&chrdevs_lock);
	for (cd = chrdevs[major_to_index(offset)]; cd; cd = cd->next) {
		if (cd->major == offset)
			seq_printf(f, "%3d %s\n", cd->major, cd->name);
	}
	mutex_unlock(&chrdevs_lock);
}