static spinlock_t *busylock_acquire(void *ptr)
{
	spinlock_t *busy;

	busy = udp_busylocks + hash_ptr(ptr, udp_busylocks_log);
	spin_lock(busy);
	return busy;
}