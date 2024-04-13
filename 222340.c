static inline void input_wakeup_procfs_readers(void)
{
	input_devices_state++;
	wake_up(&input_devices_poll_wait);
}