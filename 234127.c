static int uuid_strategy(ctl_table *table,
			 void __user *oldval, size_t __user *oldlenp,
			 void __user *newval, size_t newlen)
{
	unsigned char tmp_uuid[16], *uuid;
	unsigned int len;

	if (!oldval || !oldlenp)
		return 1;

	uuid = table->data;
	if (!uuid) {
		uuid = tmp_uuid;
		uuid[8] = 0;
	}
	if (uuid[8] == 0)
		generate_random_uuid(uuid);

	if (get_user(len, oldlenp))
		return -EFAULT;
	if (len) {
		if (len > 16)
			len = 16;
		if (copy_to_user(oldval, uuid, len) ||
		    put_user(len, oldlenp))
			return -EFAULT;
	}
	return 1;
}