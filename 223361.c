void option_error(void)
{
	if (!err_buf[0]) {
		strlcpy(err_buf, "Error parsing options: option may "
			"be supported on client but not on server?\n",
			sizeof err_buf);
	}

	rprintf(FERROR, RSYNC_NAME ": %s", err_buf);
	msleep(20);
}