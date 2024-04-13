static void fuse_file_put(struct fuse_file *ff)
{
	if (atomic_dec_and_test(&ff->count)) {
		struct fuse_req *req = ff->reserved_req;

		req->end = fuse_release_end;
		fuse_request_send_background(ff->fc, req);
		kfree(ff);
	}
}