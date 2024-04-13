static int trace_create_savedcmd(void)
{
	int ret;

	savedcmd = kmalloc(sizeof(*savedcmd), GFP_KERNEL);
	if (!savedcmd)
		return -ENOMEM;

	ret = allocate_cmdlines_buffer(SAVED_CMDLINES_DEFAULT, savedcmd);
	if (ret < 0) {
		kfree(savedcmd);
		savedcmd = NULL;
		return -ENOMEM;
	}

	return 0;
}