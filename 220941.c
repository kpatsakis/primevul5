mono_method_signature (MonoMethod *m)
{
	MonoError error;
	MonoMethodSignature *sig;

	sig = mono_method_signature_checked (m, &error);
	if (!sig) {
		char *type_name = mono_type_get_full_name (m->klass);
		g_warning ("Could not load signature of %s:%s due to: %s", type_name, m->name, mono_error_get_message (&error));
		g_free (type_name);
		mono_error_cleanup (&error);
	}

	return sig;
}