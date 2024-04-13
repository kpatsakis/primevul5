static Bool ttml_check_range(TTMLInterval *interval, s64 ts_begin, s64 ts_end)
{
	//if in current interval, push node
	if ((ts_begin != -1) && (ts_end != -1) && ((ts_begin>=interval->begin) && (ts_end<=interval->end))
	) {
		return GF_TRUE;
	}
	//begin not set, end set: in range if end less than interval end range
	else if ((ts_begin==-1) && (ts_end != -1) && (ts_end<=interval->end)) {
		return GF_TRUE;
	}
	//begin set, end not set: in range if begin greater than interval begin range
	else if ((ts_begin!=-1) && (ts_end==-1) && (ts_begin>=interval->begin)) {
		return GF_TRUE;
	}
	return GF_FALSE;
}