mono_method_get_signature (MonoMethod *method, MonoImage *image, guint32 token)
{
	return mono_method_get_signature_full (method, image, token, NULL);
}