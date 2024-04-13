static void mt_sync_frame(struct mt_device *td, struct mt_application *app,
			  struct input_dev *input)
{
	if (app->quirks & MT_QUIRK_WIN8_PTP_BUTTONS)
		input_event(input, EV_KEY, BTN_LEFT, app->left_button_state);

	input_mt_sync_frame(input);
	input_event(input, EV_MSC, MSC_TIMESTAMP, app->timestamp);
	input_sync(input);

	mt_release_pending_palms(td, app, input);

	app->num_received = 0;
	app->left_button_state = 0;

	if (test_bit(MT_IO_FLAGS_ACTIVE_SLOTS, &td->mt_io_flags))
		set_bit(MT_IO_FLAGS_PENDING_SLOTS, &td->mt_io_flags);
	else
		clear_bit(MT_IO_FLAGS_PENDING_SLOTS, &td->mt_io_flags);
	clear_bit(MT_IO_FLAGS_ACTIVE_SLOTS, &td->mt_io_flags);
}