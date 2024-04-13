gp_get_string(char * buffer, size_t len, const char * prompt)
{
# ifdef USE_READLINE
    if (interactive)
	return rlgets(buffer, len, prompt);
    else
	return fgets_ipc(buffer, len);
# else
    if (interactive)
	PUT_STRING(prompt);

    return GET_STRING(buffer, len);
# endif
}