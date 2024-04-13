static inline int add_post_vars(zval *arr, post_var_data_t *vars, zend_bool eof)
{
	uint64_t max_vars = PG(max_input_vars);

	vars->ptr = ZSTR_VAL(vars->str.s);
	vars->end = ZSTR_VAL(vars->str.s) + ZSTR_LEN(vars->str.s);
	while (add_post_var(arr, vars, eof)) {
		if (++vars->cnt > max_vars) {
			php_error_docref(NULL, E_WARNING,
					"Input variables exceeded %" PRIu64 ". "
					"To increase the limit change max_input_vars in php.ini.",
					max_vars);
			return FAILURE;
		}
	}

	if (!eof && ZSTR_VAL(vars->str.s) != vars->ptr) {
		memmove(ZSTR_VAL(vars->str.s), vars->ptr, ZSTR_LEN(vars->str.s) = vars->end - vars->ptr);
	}
	return SUCCESS;
}