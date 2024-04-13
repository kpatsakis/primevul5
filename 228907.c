lka_report_proc(const char *name, const char *line)
{
	char buffer[LINE_MAX];
	struct timeval tv;
	char *ep, *sp, *direction;
	uint64_t reqid;

	if (strlcpy(buffer, line + 7, sizeof(buffer)) >= sizeof(buffer))
		fatalx("Invalid report: line too long: %s", line);

	errno = 0;
	tv.tv_sec = strtoll(buffer, &ep, 10);
	if (ep[0] != '.' || errno != 0)
		fatalx("Invalid report: invalid time: %s", line);
	sp = ep + 1;
	tv.tv_usec = strtol(sp, &ep, 10);
	if (ep[0] != '|' || errno != 0)
		fatalx("Invalid report: invalid time: %s", line);
	if (ep - sp != 6)
		fatalx("Invalid report: invalid time: %s", line);

	direction = ep + 1;
	if (strncmp(direction, "smtp-in|", 8) == 0) {
		direction[7] = '\0';
		direction += 7;
#if 0
	} else if (strncmp(direction, "smtp-out|", 9) == 0) {
		direction[8] = '\0';
		direction += 8;
#endif
	} else
		fatalx("Invalid report: invalid direction: %s", line);

	reqid = strtoull(sp, &ep, 16);
	if (ep[0] != '|' || errno != 0)
		fatalx("Invalid report: invalid reqid: %s", line);
	sp = ep + 1;

	lka_report_filter_report(reqid, name, 0, direction, &tv, sp);
}