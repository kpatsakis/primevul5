static UINT parallel_free(DEVICE* device)
{
	UINT error;
	PARALLEL_DEVICE* parallel = (PARALLEL_DEVICE*)device;

	if (!MessageQueue_PostQuit(parallel->queue, 0) ||
	    (WaitForSingleObject(parallel->thread, INFINITE) == WAIT_FAILED))
	{
		error = GetLastError();
		WLog_ERR(TAG, "WaitForSingleObject failed with error %" PRIu32 "!", error);
		return error;
	}

	CloseHandle(parallel->thread);
	Stream_Free(parallel->device.data, TRUE);
	MessageQueue_Free(parallel->queue);
	free(parallel);
	return CHANNEL_RC_OK;
}