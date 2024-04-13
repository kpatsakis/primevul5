UINT DeviceServiceEntry(PDEVICE_SERVICE_ENTRY_POINTS pEntryPoints)
{
	char* name;
	char* path;
	size_t i;
	size_t length;
	RDPDR_PARALLEL* device;
	PARALLEL_DEVICE* parallel;
	UINT error;
	device = (RDPDR_PARALLEL*)pEntryPoints->device;
	name = device->Name;
	path = device->Path;

	if (!name || (name[0] == '*') || !path)
	{
		/* TODO: implement auto detection of parallel ports */
		return CHANNEL_RC_INITIALIZATION_ERROR;
	}

	if (name[0] && path[0])
	{
		parallel = (PARALLEL_DEVICE*)calloc(1, sizeof(PARALLEL_DEVICE));

		if (!parallel)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		parallel->device.type = RDPDR_DTYP_PARALLEL;
		parallel->device.name = name;
		parallel->device.IRPRequest = parallel_irp_request;
		parallel->device.Free = parallel_free;
		parallel->rdpcontext = pEntryPoints->rdpcontext;
		length = strlen(name);
		parallel->device.data = Stream_New(NULL, length + 1);

		if (!parallel->device.data)
		{
			WLog_ERR(TAG, "Stream_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		for (i = 0; i <= length; i++)
			Stream_Write_UINT8(parallel->device.data, name[i] < 0 ? '_' : name[i]);

		parallel->path = path;
		parallel->queue = MessageQueue_New(NULL);

		if (!parallel->queue)
		{
			WLog_ERR(TAG, "MessageQueue_New failed!");
			error = CHANNEL_RC_NO_MEMORY;
			goto error_out;
		}

		if ((error = pEntryPoints->RegisterDevice(pEntryPoints->devman, (DEVICE*)parallel)))
		{
			WLog_ERR(TAG, "RegisterDevice failed with error %" PRIu32 "!", error);
			goto error_out;
		}

		if (!(parallel->thread =
		          CreateThread(NULL, 0, parallel_thread_func, (void*)parallel, 0, NULL)))
		{
			WLog_ERR(TAG, "CreateThread failed!");
			error = ERROR_INTERNAL_ERROR;
			goto error_out;
		}
	}

	return CHANNEL_RC_OK;
error_out:
	MessageQueue_Free(parallel->queue);
	Stream_Free(parallel->device.data, TRUE);
	free(parallel);
	return error;
}