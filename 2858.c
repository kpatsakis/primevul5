static void buffer_written(struct v4l2_loopback_device *dev,
			   struct v4l2l_buffer *buf)
{
	del_timer_sync(&dev->sustain_timer);
	del_timer_sync(&dev->timeout_timer);
	spin_lock_bh(&dev->lock);

	dev->bufpos2index[dev->write_position % dev->used_buffers] =
		buf->buffer.index;
	list_move_tail(&buf->list_head, &dev->outbufs_list);
	++dev->write_position;
	dev->reread_count = 0;

	check_timers(dev);
	spin_unlock_bh(&dev->lock);
}