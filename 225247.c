static void v4l_sanitize_format(struct v4l2_format *fmt)
{
	unsigned int offset;

	/* Make sure num_planes is not bogus */
	if (fmt->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE ||
	    fmt->type == V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE)
		fmt->fmt.pix_mp.num_planes = min_t(u32, fmt->fmt.pix_mp.num_planes,
					       VIDEO_MAX_PLANES);

	/*
	 * The v4l2_pix_format structure has been extended with fields that were
	 * not previously required to be set to zero by applications. The priv
	 * field, when set to a magic value, indicates the the extended fields
	 * are valid. Otherwise they will contain undefined values. To simplify
	 * the API towards drivers zero the extended fields and set the priv
	 * field to the magic value when the extended pixel format structure
	 * isn't used by applications.
	 */

	if (fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE &&
	    fmt->type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
		return;

	if (fmt->fmt.pix.priv == V4L2_PIX_FMT_PRIV_MAGIC)
		return;

	fmt->fmt.pix.priv = V4L2_PIX_FMT_PRIV_MAGIC;

	offset = offsetof(struct v4l2_pix_format, priv)
	       + sizeof(fmt->fmt.pix.priv);
	memset(((void *)&fmt->fmt.pix) + offset, 0,
	       sizeof(fmt->fmt.pix) - offset);
}