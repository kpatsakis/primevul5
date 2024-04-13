static MSUSB_PIPE_DESCRIPTOR* msusb_mspipe_new()
{
	return (MSUSB_PIPE_DESCRIPTOR*)calloc(1, sizeof(MSUSB_PIPE_DESCRIPTOR));
}