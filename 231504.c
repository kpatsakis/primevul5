ssize_t trace_parse_run_command(struct file *file, const char __user *buffer,
				size_t count, loff_t *ppos,
				int (*createfn)(int, char **))
{
	char *kbuf, *buf, *tmp;
	int ret = 0;
	size_t done = 0;
	size_t size;

	kbuf = kmalloc(WRITE_BUFSIZE, GFP_KERNEL);
	if (!kbuf)
		return -ENOMEM;

	while (done < count) {
		size = count - done;

		if (size >= WRITE_BUFSIZE)
			size = WRITE_BUFSIZE - 1;

		if (copy_from_user(kbuf, buffer + done, size)) {
			ret = -EFAULT;
			goto out;
		}
		kbuf[size] = '\0';
		buf = kbuf;
		do {
			tmp = strchr(buf, '\n');
			if (tmp) {
				*tmp = '\0';
				size = tmp - buf + 1;
			} else {
				size = strlen(buf);
				if (done + size < count) {
					if (buf != kbuf)
						break;
					/* This can accept WRITE_BUFSIZE - 2 ('\n' + '\0') */
					pr_warn("Line length is too long: Should be less than %d\n",
						WRITE_BUFSIZE - 2);
					ret = -EINVAL;
					goto out;
				}
			}
			done += size;

			/* Remove comments */
			tmp = strchr(buf, '#');

			if (tmp)
				*tmp = '\0';

			ret = trace_run_command(buf, createfn);
			if (ret)
				goto out;
			buf += size;

		} while (done < count);
	}
	ret = done;

out:
	kfree(kbuf);

	return ret;
}