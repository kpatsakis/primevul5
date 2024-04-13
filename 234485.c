enh_err_check(const char *str)
{
    if (*str == '}')
	int_warn(NO_CARET, "enhanced text mode parser - ignoring spurious }");
    else
	int_warn(NO_CARET, "enhanced text mode parsing error");
}