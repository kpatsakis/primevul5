static void v4l_print_event(const void *arg, bool write_only)
{
	const struct v4l2_event *p = arg;
	const struct v4l2_event_ctrl *c;

	pr_cont("type=0x%x, pending=%u, sequence=%u, id=%u, timestamp=%llu.%9.9llu\n",
			p->type, p->pending, p->sequence, p->id,
			p->timestamp.tv_sec, p->timestamp.tv_nsec);
	switch (p->type) {
	case V4L2_EVENT_VSYNC:
		printk(KERN_DEBUG "field=%s\n",
			prt_names(p->u.vsync.field, v4l2_field_names));
		break;
	case V4L2_EVENT_CTRL:
		c = &p->u.ctrl;
		printk(KERN_DEBUG "changes=0x%x, type=%u, ",
			c->changes, c->type);
		if (c->type == V4L2_CTRL_TYPE_INTEGER64)
			pr_cont("value64=%lld, ", c->value64);
		else
			pr_cont("value=%d, ", c->value);
		pr_cont("flags=0x%x, minimum=%d, maximum=%d, step=%d, default_value=%d\n",
			c->flags, c->minimum, c->maximum,
			c->step, c->default_value);
		break;
	case V4L2_EVENT_FRAME_SYNC:
		pr_cont("frame_sequence=%u\n",
			p->u.frame_sync.frame_sequence);
		break;
	}
}