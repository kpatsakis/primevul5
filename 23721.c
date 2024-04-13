static void fuse_release_end(struct fuse_conn *fc, struct fuse_req *req)
{
	path_put(&req->misc.release.path);
}