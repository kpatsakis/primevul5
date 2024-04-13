_evbuffer_testing_use_mmap(void)
{
	int ok = 0;
#ifdef USE_SENDFILE
	use_sendfile = 0;
#endif
#ifdef _EVENT_HAVE_MMAP
	use_mmap = 1;
	ok = 1;
#endif
	return ok;
}