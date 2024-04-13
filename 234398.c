string_expand_macros()
{
	if (expand_1level_macros() && expand_1level_macros()
	&&  expand_1level_macros() && expand_1level_macros())
	    int_error(NO_CARET, "Macros nested too deeply");
}