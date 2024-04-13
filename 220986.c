mono_get_method_constrained (MonoImage *image, guint32 token, MonoClass *constrained_class,
			     MonoGenericContext *context, MonoMethod **cil_method)
{
	MonoMethod *method, *result;
	MonoClass *ic = NULL;
	MonoGenericContext *method_context = NULL;
	MonoMethodSignature *sig, *original_sig;

	mono_loader_lock ();

	*cil_method = mono_get_method_from_token (image, token, NULL, context, NULL);
	if (!*cil_method) {
		mono_loader_unlock ();
		return NULL;
	}

	mono_class_init (constrained_class);
	method = *cil_method;
	original_sig = sig = mono_method_signature (method);
	if (sig == NULL) {
		mono_loader_unlock ();
		return NULL;
	}

	if (method->is_inflated && sig->generic_param_count) {
		MonoMethodInflated *imethod = (MonoMethodInflated *) method;
		sig = mono_method_signature (imethod->declaring); /*We assume that if the inflated method signature is valid, the declaring method is too*/
		method_context = mono_method_get_context (method);

		original_sig = sig;
		/*
		 * We must inflate the signature with the class instantiation to work on
		 * cases where a class inherit from a generic type and the override replaces
		 * any type argument which a concrete type. See #325283.
		 */
		if (method_context->class_inst) {
			MonoError error;
			MonoGenericContext ctx;
			ctx.method_inst = NULL;
			ctx.class_inst = method_context->class_inst;
			/*Fixme, property propagate this error*/
			sig = inflate_generic_signature_checked (method->klass->image, sig, &ctx, &error);
			if (!mono_error_ok (&error)) {
				mono_loader_unlock ();
				mono_error_cleanup (&error);
				return NULL;
			}
		}
	}

	if ((constrained_class != method->klass) && (MONO_CLASS_IS_INTERFACE (method->klass)))
		ic = method->klass;

	result = find_method (constrained_class, ic, method->name, sig, constrained_class);
	if (sig != original_sig)
		mono_metadata_free_inflated_signature (sig);

	if (!result) {
		g_warning ("Missing method %s.%s.%s in assembly %s token %x", method->klass->name_space,
			   method->klass->name, method->name, image->name, token);
		mono_loader_unlock ();
		return NULL;
	}

	if (method_context)
		result = mono_class_inflate_generic_method (result, method_context);

	mono_loader_unlock ();
	return result;
}