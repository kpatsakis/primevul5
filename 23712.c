static void fuse_writepage_free(struct fuse_conn *fc, struct fuse_req *req)
{
	__free_page(req->pages[0]);
	fuse_file_put(req->ff);
}