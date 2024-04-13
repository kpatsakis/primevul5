mono_loader_error_prepare_exception (MonoLoaderError *error)
{
	MonoException *ex = NULL;

	switch (error->exception_type) {
	case MONO_EXCEPTION_TYPE_LOAD: {
		char *cname = g_strdup (error->class_name);
		char *aname = g_strdup (error->assembly_name);
		MonoString *class_name;
		
		mono_loader_clear_error ();
		
		class_name = mono_string_new (mono_domain_get (), cname);

		ex = mono_get_exception_type_load (class_name, aname);
		g_free (cname);
		g_free (aname);
		break;
        }
	case MONO_EXCEPTION_MISSING_METHOD: {
		char *cname = g_strdup (error->class_name);
		char *aname = g_strdup (error->member_name);
		
		mono_loader_clear_error ();
		ex = mono_get_exception_missing_method (cname, aname);
		g_free (cname);
		g_free (aname);
		break;
	}
		
	case MONO_EXCEPTION_MISSING_FIELD: {
		char *cnspace = g_strdup ((error->klass && *error->klass->name_space) ? error->klass->name_space : "");
		char *cname = g_strdup (error->klass ? error->klass->name : "");
		char *cmembername = g_strdup (error->member_name);
                char *class_name;

		mono_loader_clear_error ();
		class_name = g_strdup_printf ("%s%s%s", cnspace, cnspace ? "." : "", cname);
		
		ex = mono_get_exception_missing_field (class_name, cmembername);
		g_free (class_name);
		g_free (cname);
		g_free (cmembername);
		g_free (cnspace);
		break;
        }
	
	case MONO_EXCEPTION_FILE_NOT_FOUND: {
		char *msg;
		char *filename;

		if (error->ref_only)
			msg = g_strdup_printf ("Cannot resolve dependency to assembly '%s' because it has not been preloaded. When using the ReflectionOnly APIs, dependent assemblies must be pre-loaded or loaded on demand through the ReflectionOnlyAssemblyResolve event.", error->assembly_name);
		else
			msg = g_strdup_printf ("Could not load file or assembly '%s' or one of its dependencies.", error->assembly_name);
		filename = g_strdup (error->assembly_name);
		/* Has to call this before calling anything which might call mono_class_init () */
		mono_loader_clear_error ();
		ex = mono_get_exception_file_not_found2 (msg, mono_string_new (mono_domain_get (), filename));
		g_free (msg);
		g_free (filename);
		break;
	}

	case MONO_EXCEPTION_BAD_IMAGE: {
		char *msg = g_strdup (error->msg);
		mono_loader_clear_error ();
		ex = mono_get_exception_bad_image_format (msg);
		g_free (msg);
		break;
	}

	default:
		g_assert_not_reached ();
	}

	return ex;
}