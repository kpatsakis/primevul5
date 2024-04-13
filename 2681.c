default_threadlock(int acquire)
{
#ifdef ENABLE_THREAD_SAFETY
#ifndef WIN32
	static pthread_mutex_t singlethread_lock = PTHREAD_MUTEX_INITIALIZER;
#else
	static pthread_mutex_t singlethread_lock = NULL;
	static long mutex_initlock = 0;

	if (singlethread_lock == NULL)
	{
		while (InterlockedExchange(&mutex_initlock, 1) == 1)
			 /* loop, another thread own the lock */ ;
		if (singlethread_lock == NULL)
		{
			if (pthread_mutex_init(&singlethread_lock, NULL))
				Assert(false);
		}
		InterlockedExchange(&mutex_initlock, 0);
	}
#endif
	if (acquire)
	{
		if (pthread_mutex_lock(&singlethread_lock))
			Assert(false);
	}
	else
	{
		if (pthread_mutex_unlock(&singlethread_lock))
			Assert(false);
	}
#endif
}