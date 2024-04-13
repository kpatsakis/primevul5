static UINT parallel_process_irp_read(PARALLEL_DEVICE* parallel, IRP* irp)
{
	UINT32 Length;
	UINT64 Offset;
	ssize_t status;
	BYTE* buffer = NULL;
	if (Stream_GetRemainingLength(irp->input) < 12)
		return ERROR_INVALID_DATA;
	Stream_Read_UINT32(irp->input, Length);
	Stream_Read_UINT64(irp->input, Offset);
	buffer = (BYTE*)malloc(Length);

	if (!buffer)
	{
		WLog_ERR(TAG, "malloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	status = read(parallel->file, buffer, Length);

	if (status < 0)
	{
		irp->IoStatus = STATUS_UNSUCCESSFUL;
		free(buffer);
		buffer = NULL;
		Length = 0;
	}
	else
	{
	}

	Stream_Write_UINT32(irp->output, Length);

	if (Length > 0)
	{
		if (!Stream_EnsureRemainingCapacity(irp->output, Length))
		{
			WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
			free(buffer);
			return CHANNEL_RC_NO_MEMORY;
		}

		Stream_Write(irp->output, buffer, Length);
	}

	free(buffer);
	return irp->Complete(irp);
}