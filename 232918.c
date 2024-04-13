static void busylock_release(spinlock_t *busy)
{
	if (busy)
		spin_unlock(busy);
}