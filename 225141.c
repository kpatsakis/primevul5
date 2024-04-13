static void v4l_print_decoder_cmd(const void *arg, bool write_only)
{
	const struct v4l2_decoder_cmd *p = arg;

	pr_cont("cmd=%d, flags=0x%x\n", p->cmd, p->flags);

	if (p->cmd == V4L2_DEC_CMD_START)
		pr_info("speed=%d, format=%u\n",
				p->start.speed, p->start.format);
	else if (p->cmd == V4L2_DEC_CMD_STOP)
		pr_info("pts=%llu\n", p->stop.pts);
}