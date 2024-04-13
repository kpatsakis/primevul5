mono_get_method (MonoImage *image, guint32 token, MonoClass *klass)
{
	return mono_get_method_full (image, token, klass, NULL);
}