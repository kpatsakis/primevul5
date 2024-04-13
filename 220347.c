int tty_dev_name_to_number(const char *name, dev_t *number)
{
	struct tty_driver *p;
	int ret;
	int index, prefix_length = 0;
	const char *str;

	for (str = name; *str && !isdigit(*str); str++)
		;

	if (!*str)
		return -EINVAL;

	ret = kstrtoint(str, 10, &index);
	if (ret)
		return ret;

	prefix_length = str - name;
	mutex_lock(&tty_mutex);

	list_for_each_entry(p, &tty_drivers, tty_drivers)
		if (prefix_length == strlen(p->name) && strncmp(name,
					p->name, prefix_length) == 0) {
			if (index < p->num) {
				*number = MKDEV(p->major, p->minor_start + index);
				goto out;
			}
		}

	/* if here then driver wasn't found */
	ret = -ENODEV;
out:
	mutex_unlock(&tty_mutex);
	return ret;
}