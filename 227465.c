static UINT parallel_irp_request(DEVICE* device, IRP* irp)
{
	PARALLEL_DEVICE* parallel = (PARALLEL_DEVICE*)device;

	if (!MessageQueue_Post(parallel->queue, NULL, 0, (void*)irp, NULL))
	{
		WLog_ERR(TAG, "MessageQueue_Post failed!");
		return ERROR_INTERNAL_ERROR;
	}

	return CHANNEL_RC_OK;
}