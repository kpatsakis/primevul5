#else
static inline void bfq_update_insert_stats(struct request_queue *q,
					   struct bfq_queue *bfqq,
					   bool idle_timer_disabled,