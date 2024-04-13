static const struct v4l2l_format *format_by_fourcc(int fourcc)
{
	unsigned int i;

	for (i = 0; i < FORMATS; i++) {
		if (formats[i].fourcc == fourcc)
			return formats + i;
	}

	dprintk("unsupported format '%c%c%c%c'\n", (fourcc >> 0) & 0xFF,
		(fourcc >> 8) & 0xFF, (fourcc >> 16) & 0xFF,
		(fourcc >> 24) & 0xFF);
	return NULL;
}