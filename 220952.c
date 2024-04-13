mono_lookup_pinvoke_call (MonoMethod *method, const char **exc_class, const char **exc_arg)
{
	MonoImage *image = method->klass->image;
	MonoMethodPInvoke *piinfo = (MonoMethodPInvoke *)method;
	MonoTableInfo *tables = image->tables;
	MonoTableInfo *im = &tables [MONO_TABLE_IMPLMAP];
	MonoTableInfo *mr = &tables [MONO_TABLE_MODULEREF];
	guint32 im_cols [MONO_IMPLMAP_SIZE];
	guint32 scope_token;
	const char *import = NULL;
	const char *orig_scope;
	const char *new_scope;
	char *error_msg;
	char *full_name, *file_name;
	int i;
	MonoDl *module = NULL;

	g_assert (method->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL);

	if (piinfo->addr)
		return piinfo->addr;

	if (method->klass->image->dynamic) {
		MonoReflectionMethodAux *method_aux = 
			g_hash_table_lookup (
				((MonoDynamicImage*)method->klass->image)->method_aux_hash, method);
		if (!method_aux)
			return NULL;

		import = method_aux->dllentry;
		orig_scope = method_aux->dll;
	}
	else {
		if (!piinfo->implmap_idx)
			return NULL;

		mono_metadata_decode_row (im, piinfo->implmap_idx - 1, im_cols, MONO_IMPLMAP_SIZE);

		piinfo->piflags = im_cols [MONO_IMPLMAP_FLAGS];
		import = mono_metadata_string_heap (image, im_cols [MONO_IMPLMAP_NAME]);
		scope_token = mono_metadata_decode_row_col (mr, im_cols [MONO_IMPLMAP_SCOPE] - 1, MONO_MODULEREF_NAME);
		orig_scope = mono_metadata_string_heap (image, scope_token);
	}

	mono_dllmap_lookup (image, orig_scope, import, &new_scope, &import);

	mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
			"DllImport attempting to load: '%s'.", new_scope);

	if (exc_class) {
		*exc_class = NULL;
		*exc_arg = NULL;
	}

	/* we allow a special name to dlopen from the running process namespace */
	if (strcmp (new_scope, "__Internal") == 0)
		module = mono_dl_open (NULL, MONO_DL_LAZY, &error_msg);

	/*
	 * Try loading the module using a variety of names
	 */
	for (i = 0; i < 4; ++i) {
		switch (i) {
		case 0:
			/* Try the original name */
			file_name = g_strdup (new_scope);
			break;
		case 1:
			/* Try trimming the .dll extension */
			if (strstr (new_scope, ".dll") == (new_scope + strlen (new_scope) - 4)) {
				file_name = g_strdup (new_scope);
				file_name [strlen (new_scope) - 4] = '\0';
			}
			else
				continue;
			break;
		case 2:
			if (strstr (new_scope, "lib") != new_scope) {
				file_name = g_strdup_printf ("lib%s", new_scope);
			}
			else
				continue;
			break;
		default:
#ifndef TARGET_WIN32
			if (!g_ascii_strcasecmp ("user32.dll", new_scope) ||
			    !g_ascii_strcasecmp ("kernel32.dll", new_scope) ||
			    !g_ascii_strcasecmp ("user32", new_scope) ||
			    !g_ascii_strcasecmp ("kernel", new_scope)) {
				file_name = g_strdup ("libMonoSupportW.so");
			} else
#endif
				    continue;
#ifndef TARGET_WIN32
			break;
#endif
		}

		if (!module) {
			void *iter = NULL;
			char *mdirname = g_path_get_dirname (image->name);
			while ((full_name = mono_dl_build_path (mdirname, file_name, &iter))) {
				mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
					"DllImport loading library: '%s'.", full_name);
				module = cached_module_load (full_name, MONO_DL_LAZY, &error_msg);
				if (!module) {
					mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
						"DllImport error loading library '%s'.",
						error_msg);
					g_free (error_msg);
				}
				g_free (full_name);
				if (module)
					break;
			}
			g_free (mdirname);
		}

		if (!module) {
			void *iter = NULL;
			while ((full_name = mono_dl_build_path (NULL, file_name, &iter))) {
				mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
						"DllImport loading location: '%s'.", full_name);
				module = cached_module_load (full_name, MONO_DL_LAZY, &error_msg);
				if (!module) {
					mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
							"DllImport error loading library: '%s'.",
							error_msg);
					g_free (error_msg);
				}
				g_free (full_name);
				if (module)
					break;
			}
		}

		if (!module) {
			mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
					"DllImport loading: '%s'.", file_name);
			module = cached_module_load (file_name, MONO_DL_LAZY, &error_msg);
			if (!module) {
				mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
						"DllImport error loading library '%s'.",
						error_msg);
			}
		}

		g_free (file_name);

		if (module)
			break;
	}

	if (!module) {
		mono_trace (G_LOG_LEVEL_WARNING, MONO_TRACE_DLLIMPORT,
				"DllImport unable to load library '%s'.",
				error_msg);
		g_free (error_msg);

		if (exc_class) {
			*exc_class = "DllNotFoundException";
			*exc_arg = new_scope;
		}
		return NULL;
	}

#ifdef TARGET_WIN32
	if (import && import [0] == '#' && isdigit (import [1])) {
		char *end;
		long id;

		id = strtol (import + 1, &end, 10);
		if (id > 0 && *end == '\0')
			import++;
	}
#endif
	mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
				"Searching for '%s'.", import);

	if (piinfo->piflags & PINVOKE_ATTRIBUTE_NO_MANGLE) {
		error_msg = mono_dl_symbol (module, import, &piinfo->addr); 
	} else {
		char *mangled_name = NULL, *mangled_name2 = NULL;
		int mangle_charset;
		int mangle_stdcall;
		int mangle_param_count;
#ifdef TARGET_WIN32
		int param_count;
#endif

		/*
		 * Search using a variety of mangled names
		 */
		for (mangle_charset = 0; mangle_charset <= 1; mangle_charset ++) {
			for (mangle_stdcall = 0; mangle_stdcall <= 1; mangle_stdcall ++) {
				gboolean need_param_count = FALSE;
#ifdef TARGET_WIN32
				if (mangle_stdcall > 0)
					need_param_count = TRUE;
#endif
				for (mangle_param_count = 0; mangle_param_count <= (need_param_count ? 256 : 0); mangle_param_count += 4) {

					if (piinfo->addr)
						continue;

					mangled_name = (char*)import;
					switch (piinfo->piflags & PINVOKE_ATTRIBUTE_CHAR_SET_MASK) {
					case PINVOKE_ATTRIBUTE_CHAR_SET_UNICODE:
						/* Try the mangled name first */
						if (mangle_charset == 0)
							mangled_name = g_strconcat (import, "W", NULL);
						break;
					case PINVOKE_ATTRIBUTE_CHAR_SET_AUTO:
#ifdef TARGET_WIN32
						if (mangle_charset == 0)
							mangled_name = g_strconcat (import, "W", NULL);
#else
						/* Try the mangled name last */
						if (mangle_charset == 1)
							mangled_name = g_strconcat (import, "A", NULL);
#endif
						break;
					case PINVOKE_ATTRIBUTE_CHAR_SET_ANSI:
					default:
						/* Try the mangled name last */
						if (mangle_charset == 1)
							mangled_name = g_strconcat (import, "A", NULL);
						break;
					}

#ifdef TARGET_WIN32
					if (mangle_param_count == 0)
						param_count = mono_method_signature (method)->param_count * sizeof (gpointer);
					else
						/* Try brute force, since it would be very hard to compute the stack usage correctly */
						param_count = mangle_param_count;

					/* Try the stdcall mangled name */
					/* 
					 * gcc under windows creates mangled names without the underscore, but MS.NET
					 * doesn't support it, so we doesn't support it either.
					 */
					if (mangle_stdcall == 1)
						mangled_name2 = g_strdup_printf ("_%s@%d", mangled_name, param_count);
					else
						mangled_name2 = mangled_name;
#else
					mangled_name2 = mangled_name;
#endif

					mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
								"Probing '%s'.", mangled_name2);

					error_msg = mono_dl_symbol (module, mangled_name2, &piinfo->addr);

					if (piinfo->addr)
						mono_trace (G_LOG_LEVEL_INFO, MONO_TRACE_DLLIMPORT,
									"Found as '%s'.", mangled_name2);

					if (mangled_name != mangled_name2)
						g_free (mangled_name2);
					if (mangled_name != import)
						g_free (mangled_name);
				}
			}
		}
	}

	if (!piinfo->addr) {
		g_free (error_msg);
		if (exc_class) {
			*exc_class = "EntryPointNotFoundException";
			*exc_arg = import;
		}
		return NULL;
	}
	return piinfo->addr;
}