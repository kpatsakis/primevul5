static void io_req_free_batch(struct req_batch *rb, struct io_kiocb *req,
			      struct io_submit_state *state)
{
	io_queue_next(req);
	io_dismantle_req(req);

	if (req->task != rb->task) {
		if (rb->task)
			io_put_task(rb->task, rb->task_refs);
		rb->task = req->task;
		rb->task_refs = 0;
	}
	rb->task_refs++;
	rb->ctx_refs++;

	if (state->free_reqs != ARRAY_SIZE(state->reqs))
		state->reqs[state->free_reqs++] = req;
	else
		list_add(&req->compl.list, &state->comp.free_list);
}