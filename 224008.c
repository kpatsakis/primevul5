static int power_pmu_event_idx(struct perf_event *event)
{
	return event->hw.idx;
}