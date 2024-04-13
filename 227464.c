static UINT parallel_process_irp(PARALLEL_DEVICE* parallel, IRP* irp)
{
	UINT error;

	switch (irp->MajorFunction)
	{
		case IRP_MJ_CREATE:
			if ((error = parallel_process_irp_create(parallel, irp)))
			{
				WLog_ERR(TAG, "parallel_process_irp_create failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_CLOSE:
			if ((error = parallel_process_irp_close(parallel, irp)))
			{
				WLog_ERR(TAG, "parallel_process_irp_close failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_READ:
			if ((error = parallel_process_irp_read(parallel, irp)))
			{
				WLog_ERR(TAG, "parallel_process_irp_read failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_WRITE:
			if ((error = parallel_process_irp_write(parallel, irp)))
			{
				WLog_ERR(TAG, "parallel_process_irp_write failed with error %" PRIu32 "!", error);
				return error;
			}

			break;

		case IRP_MJ_DEVICE_CONTROL:
			if ((error = parallel_process_irp_device_control(parallel, irp)))
			{
				WLog_ERR(TAG, "parallel_process_irp_device_control failed with error %" PRIu32 "!",
				         error);
				return error;
			}

			break;

		default:
			irp->IoStatus = STATUS_NOT_SUPPORTED;
			return irp->Complete(irp);
			break;
	}

	return CHANNEL_RC_OK;
}