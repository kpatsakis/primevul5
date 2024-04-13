static int proc_do_uuid(ctl_table *table, int write, struct file *filp,
			void __user *buffer, size_t *lenp, loff_t *ppos)
{
	ctl_table fake_table;
	unsigned char buf[64], tmp_uuid[16], *uuid;

	uuid = table->data;
	if (!uuid) {
		uuid = tmp_uuid;
		uuid[8] = 0;
	}
	if (uuid[8] == 0)
		generate_random_uuid(uuid);

	sprintf(buf, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
		"%02x%02x%02x%02x%02x%02x",
		uuid[0],  uuid[1],  uuid[2],  uuid[3],
		uuid[4],  uuid[5],  uuid[6],  uuid[7],
		uuid[8],  uuid[9],  uuid[10], uuid[11],
		uuid[12], uuid[13], uuid[14], uuid[15]);
	fake_table.data = buf;
	fake_table.maxlen = sizeof(buf);

	return proc_dostring(&fake_table, write, filp, buffer, lenp, ppos);
}