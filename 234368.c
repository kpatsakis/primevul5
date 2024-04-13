import_command()
{
    int start_token = c_token;

#ifdef HAVE_EXTERNAL_FUNCTIONS
    struct udft_entry *udf;

    int dummy_num = 0;
    char save_dummy[MAX_NUM_VAR][MAX_ID_LEN+1];

    if (!equals(++c_token + 1, "("))
	int_error(c_token, "Expecting function template");

    memcpy(save_dummy, c_dummy_var, sizeof(save_dummy));
    do {
	c_token += 2;	/* skip to the next dummy */
	copy_str(c_dummy_var[dummy_num++], c_token, MAX_ID_LEN);
    } while (equals(c_token + 1, ",") && (dummy_num < MAX_NUM_VAR));
    if (equals(++c_token, ","))
	int_error(c_token + 1, "function contains too many parameters");
    if (!equals(c_token++, ")"))
	int_error(c_token, "missing ')'");

    if (!equals(c_token, "from"))
	int_error(c_token, "Expecting 'from <sharedobj>'");
    c_token++;

    udf = dummy_func = add_udf(start_token+1);
    udf->dummy_num = dummy_num;
    free_at(udf->at);	/* In case there was a previous function by this name */

    udf->at = external_at(udf->udf_name);
    memcpy(c_dummy_var, save_dummy, sizeof(save_dummy));
    dummy_func = NULL;	/* dont let anyone else use our workspace */

    if (!udf->at)
	int_error(NO_CARET, "failed to load external function");

    /* Don't copy the definition until we know it worked */
    m_capture(&(udf->definition), start_token, c_token - 1);

#else
    while (!END_OF_COMMAND)
	c_token++;
    int_error(start_token, "This copy of gnuplot does not support plugins");
#endif
}