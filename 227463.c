static DWORD WINAPI parallel_thread_func(LPVOID arg)
{
	IRP* irp;
	wMessage message;
	PARALLEL_DEVICE* parallel = (PARALLEL_DEVICE*)arg;
	UINT error = CHANNEL_RC_OK;

	while (1)
	{
		if (!MessageQueue_Wait(parallel->queue))
		{
			WLog_ERR(TAG, "MessageQueue_Wait failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (!MessageQueue_Peek(parallel->queue, &message, TRUE))
		{
			WLog_ERR(TAG, "MessageQueue_Peek failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		if (message.id == WMQ_QUIT)
			break;

		irp = (IRP*)message.wParam;

		if ((error = parallel_process_irp(parallel, irp)))
		{
			WLog_ERR(TAG, "parallel_process_irp failed with error %" PRIu32 "!", error);
			break;
		}
	}

	if (error && parallel->rdpcontext)
		setChannelError(parallel->rdpcontext, error, "parallel_thread_func reported an error");

	ExitThread(error);
	return error;
}