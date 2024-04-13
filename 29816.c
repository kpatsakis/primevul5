window_change_handler(int sig)
{
	received_window_change_signal = 1;
	signal(SIGWINCH, window_change_handler);
}
