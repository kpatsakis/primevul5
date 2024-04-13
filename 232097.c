#else
static inline void bfq_update_dispatch_stats(struct request_queue *q,
					     struct request *rq,
					     struct bfq_queue *in_serv_queue,