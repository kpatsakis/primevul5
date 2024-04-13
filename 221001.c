static inline void timerqueue_init_head(struct timerqueue_head *head)
{
	head->rb_root = RB_ROOT_CACHED;
}