void *v4l2l_vzalloc(unsigned long size)
{
	void *data = vmalloc(size);

	memset(data, 0, size);
	return data;
}