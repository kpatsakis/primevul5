static inline void io_req_set_rsrc_node(struct io_kiocb *req)
{
	struct io_ring_ctx *ctx = req->ctx;

	if (!req->fixed_rsrc_refs) {
		req->fixed_rsrc_refs = &ctx->rsrc_node->refs;
		percpu_ref_get(req->fixed_rsrc_refs);
	}
}