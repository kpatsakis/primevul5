_evbuffer_testing_use_linear_file_access(void)
{
#ifdef USE_SENDFILE
	use_sendfile = 0;
#endif
#ifdef _EVENT_HAVE_MMAP
	use_mmap = 0;
#endif
	return 1;
}