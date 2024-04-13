static void v4l_print_buffer(const void *arg, bool write_only)
{
	const struct v4l2_buffer *p = arg;
	const struct v4l2_timecode *tc = &p->timecode;
	const struct v4l2_plane *plane;
	int i;

	pr_cont("%02d:%02d:%02d.%09ld index=%d, type=%s, request_fd=%d, flags=0x%08x, field=%s, sequence=%d, memory=%s",
			(int)p->timestamp.tv_sec / 3600,
			((int)p->timestamp.tv_sec / 60) % 60,
			((int)p->timestamp.tv_sec % 60),
			(long)p->timestamp.tv_usec,
			p->index,
			prt_names(p->type, v4l2_type_names), p->request_fd,
			p->flags, prt_names(p->field, v4l2_field_names),
			p->sequence, prt_names(p->memory, v4l2_memory_names));

	if (V4L2_TYPE_IS_MULTIPLANAR(p->type) && p->m.planes) {
		pr_cont("\n");
		for (i = 0; i < p->length; ++i) {
			plane = &p->m.planes[i];
			printk(KERN_DEBUG
				"plane %d: bytesused=%d, data_offset=0x%08x, offset/userptr=0x%lx, length=%d\n",
				i, plane->bytesused, plane->data_offset,
				plane->m.userptr, plane->length);
		}
	} else {
		pr_cont(", bytesused=%d, offset/userptr=0x%lx, length=%d\n",
			p->bytesused, p->m.userptr, p->length);
	}

	printk(KERN_DEBUG "timecode=%02d:%02d:%02d type=%d, flags=0x%08x, frames=%d, userbits=0x%08x\n",
			tc->hours, tc->minutes, tc->seconds,
			tc->type, tc->flags, tc->frames, *(__u32 *)tc->userbits);
}