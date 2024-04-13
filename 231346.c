tracing_free_buffer_write(struct file *filp, const char __user *ubuf,
			  size_t cnt, loff_t *ppos)
{
	/*
	 * There is no need to read what the user has written, this function
	 * is just to make sure that there is no error when "echo" is used
	 */

	*ppos += cnt;

	return cnt;
}