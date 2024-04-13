enter_non_blocking(void)
{
	in_non_blocking_mode = 1;
	set_nonblock(fileno(stdin));
}
