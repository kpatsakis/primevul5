mono_method_get_flags (MonoMethod *method, guint32 *iflags)
{
	if (iflags)
		*iflags = method->iflags;
	return method->flags;
}