static void mt_expired_timeout(struct timer_list *t)
{
	struct mt_device *td = from_timer(td, t, release_timer);
	struct hid_device *hdev = td->hdev;

	/*
	 * An input report came in just before we release the sticky fingers,
	 * it will take care of the sticky fingers.
	 */
	if (test_and_set_bit(MT_IO_FLAGS_RUNNING, &td->mt_io_flags))
		return;
	if (test_bit(MT_IO_FLAGS_PENDING_SLOTS, &td->mt_io_flags))
		mt_release_contacts(hdev);
	clear_bit(MT_IO_FLAGS_RUNNING, &td->mt_io_flags);
}