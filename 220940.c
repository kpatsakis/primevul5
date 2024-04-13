mono_method_get_header (MonoMethod *method)
{
	int idx;
	guint32 rva;
	MonoImage* img;
	gpointer loc;
	MonoMethodHeader *header;

	if ((method->flags & METHOD_ATTRIBUTE_ABSTRACT) || (method->iflags & METHOD_IMPL_ATTRIBUTE_RUNTIME) || (method->iflags & METHOD_IMPL_ATTRIBUTE_INTERNAL_CALL) || (method->flags & METHOD_ATTRIBUTE_PINVOKE_IMPL))
		return NULL;

	if (method->is_inflated) {
		MonoMethodInflated *imethod = (MonoMethodInflated *) method;
		MonoMethodHeader *header;

		mono_loader_lock ();

		if (imethod->header) {
			mono_loader_unlock ();
			return imethod->header;
		}

		header = mono_method_get_header (imethod->declaring);
		if (!header) {
			mono_loader_unlock ();
			return NULL;
		}

		imethod->header = inflate_generic_header (header, mono_method_get_context (method));
		mono_loader_unlock ();
		mono_metadata_free_mh (header);
		return imethod->header;
	}

	if (method->wrapper_type != MONO_WRAPPER_NONE || method->sre_method) {
		MonoMethodWrapper *mw = (MonoMethodWrapper *)method;
		g_assert (mw->header);
		return mw->header;
	}

	/* 
	 * We don't need locks here: the new header is allocated from malloc memory
	 * and is not stored anywhere in the runtime, the user needs to free it.
	 */
	g_assert (mono_metadata_token_table (method->token) == MONO_TABLE_METHOD);
	idx = mono_metadata_token_index (method->token);
	img = method->klass->image;
	rva = mono_metadata_decode_row_col (&img->tables [MONO_TABLE_METHOD], idx - 1, MONO_METHOD_RVA);

	if (!mono_verifier_verify_method_header (img, rva, NULL))
		return NULL;

	loc = mono_image_rva_map (img, rva);
	if (!loc)
		return NULL;

	header = mono_metadata_parse_mh_full (img, mono_method_get_generic_container (method), loc);

	return header;
}