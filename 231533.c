tracing_mark_raw_write(struct file *filp, const char __user *ubuf,
					size_t cnt, loff_t *fpos)
{
	struct trace_array *tr = filp->private_data;
	struct ring_buffer_event *event;
	struct ring_buffer *buffer;
	struct raw_data_entry *entry;
	const char faulted[] = "<faulted>";
	unsigned long irq_flags;
	ssize_t written;
	int size;
	int len;

#define FAULT_SIZE_ID (FAULTED_SIZE + sizeof(int))

	if (tracing_disabled)
		return -EINVAL;

	if (!(tr->trace_flags & TRACE_ITER_MARKERS))
		return -EINVAL;

	/* The marker must at least have a tag id */
	if (cnt < sizeof(unsigned int) || cnt > RAW_DATA_MAX_SIZE)
		return -EINVAL;

	if (cnt > TRACE_BUF_SIZE)
		cnt = TRACE_BUF_SIZE;

	BUILD_BUG_ON(TRACE_BUF_SIZE >= PAGE_SIZE);

	local_save_flags(irq_flags);
	size = sizeof(*entry) + cnt;
	if (cnt < FAULT_SIZE_ID)
		size += FAULT_SIZE_ID - cnt;

	buffer = tr->trace_buffer.buffer;
	event = __trace_buffer_lock_reserve(buffer, TRACE_RAW_DATA, size,
					    irq_flags, preempt_count());
	if (!event)
		/* Ring buffer disabled, return as if not open for write */
		return -EBADF;

	entry = ring_buffer_event_data(event);

	len = __copy_from_user_inatomic(&entry->id, ubuf, cnt);
	if (len) {
		entry->id = -1;
		memcpy(&entry->buf, faulted, FAULTED_SIZE);
		written = -EFAULT;
	} else
		written = cnt;

	__buffer_unlock_commit(buffer, event);

	if (written > 0)
		*fpos += written;

	return written;
}