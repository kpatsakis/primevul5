static void v4l_print_jpegcompression(const void *arg, bool write_only)
{
	const struct v4l2_jpegcompression *p = arg;

	pr_cont("quality=%d, APPn=%d, APP_len=%d, COM_len=%d, jpeg_markers=0x%x\n",
		p->quality, p->APPn, p->APP_len,
		p->COM_len, p->jpeg_markers);
}