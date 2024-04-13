define()
{
    int start_token;	/* the 1st token in the function definition */
    struct udvt_entry *udv;
    struct udft_entry *udf;
    struct value result;

    if (equals(c_token + 1, "(")) {
	/* function ! */
	int dummy_num = 0;
	struct at_type *at_tmp;
	char *tmpnam;
	char save_dummy[MAX_NUM_VAR][MAX_ID_LEN+1];
	memcpy(save_dummy, c_dummy_var, sizeof(save_dummy));
	start_token = c_token;
	do {
	    c_token += 2;	/* skip to the next dummy */
	    copy_str(c_dummy_var[dummy_num++], c_token, MAX_ID_LEN);
	} while (equals(c_token + 1, ",") && (dummy_num < MAX_NUM_VAR));
	if (equals(c_token + 1, ","))
	    int_error(c_token + 2, "function contains too many parameters");
	c_token += 3;		/* skip (, dummy, ) and = */
	if (END_OF_COMMAND)
	    int_error(c_token, "function definition expected");
	udf = dummy_func = add_udf(start_token);
	udf->dummy_num = dummy_num;
	if ((at_tmp = perm_at()) == (struct at_type *) NULL)
	    int_error(start_token, "not enough memory for function");
	if (udf->at)		/* already a dynamic a.t. there */
	    free_at(udf->at);	/* so free it first */
	udf->at = at_tmp;	/* before re-assigning it. */
	memcpy(c_dummy_var, save_dummy, sizeof(save_dummy));
	m_capture(&(udf->definition), start_token, c_token - 1);
	dummy_func = NULL;	/* dont let anyone else use our workspace */

	/* Save function definition in a user-accessible variable */
	tmpnam = gp_alloc(8+strlen(udf->udf_name), "varname");
	strcpy(tmpnam, "GPFUN_");
	strcat(tmpnam, udf->udf_name);
	fill_gpval_string(tmpnam, udf->definition);
	free(tmpnam);

    } else {
	/* variable ! */
	char *varname = gp_input_line + token[c_token].start_index;
	if (!strncmp(varname, "GPVAL_", 6)
	||  !strncmp(varname, "GPFUN_", 6)
	||  !strncmp(varname, "MOUSE_", 6))
	    int_error(c_token, "Cannot set internal variables GPVAL_ GPFUN_ MOUSE_");
	start_token = c_token;
	c_token += 2;
	udv = add_udv(start_token);
	(void) const_express(&result);
	/* Prevents memory leak if the variable name is re-used */
	free_value(&udv->udv_value);
	udv->udv_value = result;
    }
}