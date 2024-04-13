null_dashtype(int type, t_dashtype *custom_dash_pattern)
{
    (void) custom_dash_pattern;	/* ignore */
    /*
     * If the terminal does not support user-defined dashtypes all we can do
     * do is fall through to the old (pre-v5) assumption that the dashtype,
     * if any, is part of the linetype.  We also assume that the color will
     * be adjusted after this.
     */
    if (type <= 0)
	type = LT_SOLID;
    term->linetype(type);
}