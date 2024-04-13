find_method (MonoClass *in_class, MonoClass *ic, const char* name, MonoMethodSignature *sig, MonoClass *from_class)
{
	int i;
	char *qname, *fqname, *class_name;
	gboolean is_interface;
	MonoMethod *result = NULL;

	is_interface = MONO_CLASS_IS_INTERFACE (in_class);

	if (ic) {
		class_name = mono_type_get_name_full (&ic->byval_arg, MONO_TYPE_NAME_FORMAT_IL);

		qname = g_strconcat (class_name, ".", name, NULL); 
		if (ic->name_space && ic->name_space [0])
			fqname = g_strconcat (ic->name_space, ".", class_name, ".", name, NULL);
		else
			fqname = NULL;
	} else
		class_name = qname = fqname = NULL;

	while (in_class) {
		g_assert (from_class);
		result = find_method_in_class (in_class, name, qname, fqname, sig, from_class);
		if (result)
			goto out;

		if (name [0] == '.' && (!strcmp (name, ".ctor") || !strcmp (name, ".cctor")))
			break;

		g_assert (from_class->interface_offsets_count == in_class->interface_offsets_count);
		for (i = 0; i < in_class->interface_offsets_count; i++) {
			MonoClass *in_ic = in_class->interfaces_packed [i];
			MonoClass *from_ic = from_class->interfaces_packed [i];
			char *ic_qname, *ic_fqname, *ic_class_name;
			
			ic_class_name = mono_type_get_name_full (&in_ic->byval_arg, MONO_TYPE_NAME_FORMAT_IL);
			ic_qname = g_strconcat (ic_class_name, ".", name, NULL); 
			if (in_ic->name_space && in_ic->name_space [0])
				ic_fqname = g_strconcat (in_ic->name_space, ".", ic_class_name, ".", name, NULL);
			else
				ic_fqname = NULL;

			result = find_method_in_class (in_ic, ic ? name : NULL, ic_qname, ic_fqname, sig, from_ic);
			g_free (ic_class_name);
			g_free (ic_fqname);
			g_free (ic_qname);
			if (result)
				goto out;
		}

		in_class = in_class->parent;
		from_class = from_class->parent;
	}
	g_assert (!in_class == !from_class);

	if (is_interface)
		result = find_method_in_class (mono_defaults.object_class, name, qname, fqname, sig, mono_defaults.object_class);

 out:
	g_free (class_name);
	g_free (fqname);
	g_free (qname);
	return result;
}