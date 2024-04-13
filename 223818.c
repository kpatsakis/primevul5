srs_timestamp_create(srs_t *srs __attribute__((unused)), char *buf, time_t now)
{
	now = now / SRS_TIME_PRECISION;
	buf[1] = SRS_TIME_BASECHARS[now & ((1 << SRS_TIME_BASEBITS) - 1)];
	now = now >> SRS_TIME_BASEBITS;
	buf[0] = SRS_TIME_BASECHARS[now & ((1 << SRS_TIME_BASEBITS) - 1)];
	buf[2] = '\0';
	return SRS_SUCCESS;
}