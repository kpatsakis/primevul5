leave_non_blocking(void)
{
	if (in_non_blocking_mode) {
		unset_nonblock(fileno(stdin));
		in_non_blocking_mode = 0;
	}
}
