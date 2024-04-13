static void v4l_print_format(const void *arg, bool write_only)
{
	const struct v4l2_format *p = arg;
	const struct v4l2_pix_format *pix;
	const struct v4l2_pix_format_mplane *mp;
	const struct v4l2_vbi_format *vbi;
	const struct v4l2_sliced_vbi_format *sliced;
	const struct v4l2_window *win;
	const struct v4l2_sdr_format *sdr;
	const struct v4l2_meta_format *meta;
	u32 planes;
	unsigned i;

	pr_cont("type=%s", prt_names(p->type, v4l2_type_names));
	switch (p->type) {
	case V4L2_BUF_TYPE_VIDEO_CAPTURE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT:
		pix = &p->fmt.pix;
		pr_cont(", width=%u, height=%u, pixelformat=%c%c%c%c, field=%s, bytesperline=%u, sizeimage=%u, colorspace=%d, flags=0x%x, ycbcr_enc=%u, quantization=%u, xfer_func=%u\n",
			pix->width, pix->height,
			(pix->pixelformat & 0xff),
			(pix->pixelformat >>  8) & 0xff,
			(pix->pixelformat >> 16) & 0xff,
			(pix->pixelformat >> 24) & 0xff,
			prt_names(pix->field, v4l2_field_names),
			pix->bytesperline, pix->sizeimage,
			pix->colorspace, pix->flags, pix->ycbcr_enc,
			pix->quantization, pix->xfer_func);
		break;
	case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
		mp = &p->fmt.pix_mp;
		pr_cont(", width=%u, height=%u, format=%c%c%c%c, field=%s, colorspace=%d, num_planes=%u, flags=0x%x, ycbcr_enc=%u, quantization=%u, xfer_func=%u\n",
			mp->width, mp->height,
			(mp->pixelformat & 0xff),
			(mp->pixelformat >>  8) & 0xff,
			(mp->pixelformat >> 16) & 0xff,
			(mp->pixelformat >> 24) & 0xff,
			prt_names(mp->field, v4l2_field_names),
			mp->colorspace, mp->num_planes, mp->flags,
			mp->ycbcr_enc, mp->quantization, mp->xfer_func);
		planes = min_t(u32, mp->num_planes, VIDEO_MAX_PLANES);
		for (i = 0; i < planes; i++)
			printk(KERN_DEBUG "plane %u: bytesperline=%u sizeimage=%u\n", i,
					mp->plane_fmt[i].bytesperline,
					mp->plane_fmt[i].sizeimage);
		break;
	case V4L2_BUF_TYPE_VIDEO_OVERLAY:
	case V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY:
		win = &p->fmt.win;
		/* Note: we can't print the clip list here since the clips
		 * pointer is a userspace pointer, not a kernelspace
		 * pointer. */
		pr_cont(", wxh=%dx%d, x,y=%d,%d, field=%s, chromakey=0x%08x, clipcount=%u, clips=%p, bitmap=%p, global_alpha=0x%02x\n",
			win->w.width, win->w.height, win->w.left, win->w.top,
			prt_names(win->field, v4l2_field_names),
			win->chromakey, win->clipcount, win->clips,
			win->bitmap, win->global_alpha);
		break;
	case V4L2_BUF_TYPE_VBI_CAPTURE:
	case V4L2_BUF_TYPE_VBI_OUTPUT:
		vbi = &p->fmt.vbi;
		pr_cont(", sampling_rate=%u, offset=%u, samples_per_line=%u, sample_format=%c%c%c%c, start=%u,%u, count=%u,%u\n",
			vbi->sampling_rate, vbi->offset,
			vbi->samples_per_line,
			(vbi->sample_format & 0xff),
			(vbi->sample_format >>  8) & 0xff,
			(vbi->sample_format >> 16) & 0xff,
			(vbi->sample_format >> 24) & 0xff,
			vbi->start[0], vbi->start[1],
			vbi->count[0], vbi->count[1]);
		break;
	case V4L2_BUF_TYPE_SLICED_VBI_CAPTURE:
	case V4L2_BUF_TYPE_SLICED_VBI_OUTPUT:
		sliced = &p->fmt.sliced;
		pr_cont(", service_set=0x%08x, io_size=%d\n",
				sliced->service_set, sliced->io_size);
		for (i = 0; i < 24; i++)
			printk(KERN_DEBUG "line[%02u]=0x%04x, 0x%04x\n", i,
				sliced->service_lines[0][i],
				sliced->service_lines[1][i]);
		break;
	case V4L2_BUF_TYPE_SDR_CAPTURE:
	case V4L2_BUF_TYPE_SDR_OUTPUT:
		sdr = &p->fmt.sdr;
		pr_cont(", pixelformat=%c%c%c%c\n",
			(sdr->pixelformat >>  0) & 0xff,
			(sdr->pixelformat >>  8) & 0xff,
			(sdr->pixelformat >> 16) & 0xff,
			(sdr->pixelformat >> 24) & 0xff);
		break;
	case V4L2_BUF_TYPE_META_CAPTURE:
	case V4L2_BUF_TYPE_META_OUTPUT:
		meta = &p->fmt.meta;
		pr_cont(", dataformat=%c%c%c%c, buffersize=%u\n",
			(meta->dataformat >>  0) & 0xff,
			(meta->dataformat >>  8) & 0xff,
			(meta->dataformat >> 16) & 0xff,
			(meta->dataformat >> 24) & 0xff,
			meta->buffersize);
		break;
	}
}