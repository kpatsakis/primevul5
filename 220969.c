mono_loader_set_error_bad_image (char *msg)
{
	MonoLoaderError *error;

	if (mono_loader_get_last_error ())
		return;

	error = g_new0 (MonoLoaderError, 1);
	error->exception_type = MONO_EXCEPTION_BAD_IMAGE;
	error->msg = msg;

	set_loader_error (error);
}	