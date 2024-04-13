print_lan_set_bad_pass_thresh_usage(void)
{
	lprintf(LOG_NOTICE,
"lan set <channel> bad_pass_thresh <thresh_num> <1|0> <reset_interval> <lockout_interval>\n"
"        <thresh_num>         Bad Password Threshold number.\n"
"        <1|0>                1 = generate a Session Audit sensor event.\n"
"                             0 = do not generate an event.\n"
"        <reset_interval>     Attempt Count Reset Interval. In tens of seconds.\n"
"        <lockount_interval>  User Lockout Interval. In tens of seconds.");
}