static void v4l_print_encoder_cmd(const void *arg, bool write_only)
{
	const struct v4l2_encoder_cmd *p = arg;

	pr_cont("cmd=%d, flags=0x%x\n",
			p->cmd, p->flags);
}