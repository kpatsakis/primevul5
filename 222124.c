_evbuffer_testing_use_sendfile(void)
{
	int ok = 0;
#ifdef USE_SENDFILE
	use_sendfile = 1;
	ok = 1;
#endif
#ifdef _EVENT_HAVE_MMAP
	use_mmap = 0;
#endif
	return ok;
}