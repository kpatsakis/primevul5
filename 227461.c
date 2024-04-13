static UINT parallel_process_irp_write(PARALLEL_DEVICE* parallel, IRP* irp)
{
	UINT32 len;
	UINT32 Length;
	UINT64 Offset;
	ssize_t status;
	void* ptr;
	if (Stream_GetRemainingLength(irp->input) > 12)
		return ERROR_INVALID_DATA;

	Stream_Read_UINT32(irp->input, Length);
	Stream_Read_UINT64(irp->input, Offset);
	if (!Stream_SafeSeek(irp->input, 20)) /* Padding */
		return ERROR_INVALID_DATA;
	ptr = Stream_Pointer(irp->input);
	if (!Stream_SafeSeek(irp->input, Length))
		return ERROR_INVALID_DATA;
	len = Length;

	while (len > 0)
	{
		status = write(parallel->file, ptr, len);

		if (status < 0)
		{
			irp->IoStatus = STATUS_UNSUCCESSFUL;
			Length = 0;
			break;
		}

		Stream_Seek(irp->input, status);
		len -= status;
	}

	Stream_Write_UINT32(irp->output, Length);
	Stream_Write_UINT8(irp->output, 0); /* Padding */
	return irp->Complete(irp);
}