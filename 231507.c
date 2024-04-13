static ssize_t trace_seq_to_buffer(struct trace_seq *s, void *buf, size_t cnt)
{
	int len;

	if (trace_seq_used(s) <= s->seq.readpos)
		return -EBUSY;

	len = trace_seq_used(s) - s->seq.readpos;
	if (cnt > len)
		cnt = len;
	memcpy(buf, s->buffer + s->seq.readpos, cnt);

	s->seq.readpos += cnt;
	return cnt;
}