 struct crypto_template *crypto_lookup_template(const char *name)
 {
	return try_then_request_module(__crypto_lookup_template(name), "%s",
				       name);
 }
