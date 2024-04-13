static void v4l_pix_format_touch(struct v4l2_pix_format *p)
{
	/*
	 * The v4l2_pix_format structure contains fields that make no sense for
	 * touch. Set them to default values in this case.
	 */

	p->field = V4L2_FIELD_NONE;
	p->colorspace = V4L2_COLORSPACE_RAW;
	p->flags = 0;
	p->ycbcr_enc = 0;
	p->quantization = 0;
	p->xfer_func = 0;
}