mono_method_signature_checked (MonoMethod *m, MonoError *error)
{
	int idx;
	int size;
	MonoImage* img;
	const char *sig;
	gboolean can_cache_signature;
	MonoGenericContainer *container;
	MonoMethodSignature *signature = NULL;
	guint32 sig_offset;

	/* We need memory barriers below because of the double-checked locking pattern */ 

	mono_error_init (error);

	if (m->signature)
		return m->signature;

	mono_loader_lock ();

	if (m->signature) {
		mono_loader_unlock ();
		return m->signature;
	}

	if (m->is_inflated) {
		MonoMethodInflated *imethod = (MonoMethodInflated *) m;
		/* the lock is recursive */
		signature = mono_method_signature (imethod->declaring);
		signature = inflate_generic_signature_checked (imethod->declaring->klass->image, signature, mono_method_get_context (m), error);
		if (!mono_error_ok (error)) {
			mono_loader_unlock ();
			return NULL;
		}

		inflated_signatures_size += mono_metadata_signature_size (signature);

		mono_memory_barrier ();
		m->signature = signature;
		mono_loader_unlock ();
		return m->signature;
	}

	g_assert (mono_metadata_token_table (m->token) == MONO_TABLE_METHOD);
	idx = mono_metadata_token_index (m->token);
	img = m->klass->image;

	sig = mono_metadata_blob_heap (img, sig_offset = mono_metadata_decode_row_col (&img->tables [MONO_TABLE_METHOD], idx - 1, MONO_METHOD_SIGNATURE));

	g_assert (!m->klass->generic_class);
	container = mono_method_get_generic_container (m);
	if (!container)
		container = m->klass->generic_container;

	/* Generic signatures depend on the container so they cannot be cached */
	/* icall/pinvoke signatures cannot be cached cause we modify them below */
	can_cache_signature = !(m->iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) && !(m->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) && !container;

	/* If the method has parameter attributes, that can modify the signature */
	if (mono_metadata_method_has_param_attrs (img, idx))
		can_cache_signature = FALSE;

	if (can_cache_signature)
		signature = g_hash_table_lookup (img->method_signatures, sig);

	if (!signature) {
		const char *sig_body;
		/*TODO we should cache the failure result somewhere*/
		if (!mono_verifier_verify_method_signature (img, sig_offset, error)) {
			mono_loader_unlock ();
			return NULL;
		}

		size = mono_metadata_decode_blob_size (sig, &sig_body);

		signature = mono_metadata_parse_method_signature_full (img, container, idx, sig_body, NULL);
		if (!signature) {
			mono_loader_unlock ();
			mono_error_set_method_load (error, m->klass, m->name, "");
			return NULL;
		}

		if (can_cache_signature)
			g_hash_table_insert (img->method_signatures, (gpointer)sig, signature);
	}

	/* Verify metadata consistency */
	if (signature->generic_param_count) {
		if (!container || !container->is_method) {
			mono_loader_unlock ();
			mono_error_set_method_load (error, m->klass, m->name, "Signature claims method has generic parameters, but generic_params table says it doesn't for method 0x%08x from image %s", idx, img->name);
			return NULL;
		}
		if (container->type_argc != signature->generic_param_count) {
			mono_loader_unlock ();
			mono_error_set_method_load (error, m->klass, m->name, "Inconsistent generic parameter count.  Signature says %d, generic_params table says %d for method 0x%08x from image %s", signature->generic_param_count, container->type_argc, idx, img->name);
			return NULL;
		}
	} else if (container && container->is_method && container->type_argc) {
		mono_loader_unlock ();
		mono_error_set_method_load (error, m->klass, m->name, "generic_params table claims method has generic parameters, but signature says it doesn't for method 0x%08x from image %s", idx, img->name);
		return NULL;
	}
	if (m->iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL)
		signature->pinvoke = 1;
	else if (m->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL) {
		MonoCallConvention conv = 0;
		MonoMethodPInvoke *piinfo = (MonoMethodPInvoke *)m;
		signature->pinvoke = 1;

		switch (piinfo->piflags & PINVOKE_ATTRIBUTE_CALL_CONV_MASK) {
		case 0: /* no call conv, so using default */
		case PINVOKE_ATTRIBUTE_CALL_CONV_WINAPI:
			conv = MONO_CALL_DEFAULT;
			break;
		case PINVOKE_ATTRIBUTE_CALL_CONV_CDECL:
			conv = MONO_CALL_C;
			break;
		case PINVOKE_ATTRIBUTE_CALL_CONV_STDCALL:
			conv = MONO_CALL_STDCALL;
			break;
		case PINVOKE_ATTRIBUTE_CALL_CONV_THISCALL:
			conv = MONO_CALL_THISCALL;
			break;
		case PINVOKE_ATTRIBUTE_CALL_CONV_FASTCALL:
			conv = MONO_CALL_FASTCALL;
			break;
		case PINVOKE_ATTRIBUTE_CALL_CONV_GENERIC:
		case PINVOKE_ATTRIBUTE_CALL_CONV_GENERICINST:
		default:
			mono_loader_unlock ();
			mono_error_set_method_load (error, m->klass, m->name, "unsupported calling convention : 0x%04x for method 0x%08x from image %s", piinfo->piflags, idx, img->name);
			return NULL;
		}
		signature->call_convention = conv;
	}

	mono_memory_barrier ();
	m->signature = signature;

	mono_loader_unlock ();
	return m->signature;
}