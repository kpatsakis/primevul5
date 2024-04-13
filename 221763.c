static void fuse_send_destroy(struct fuse_mount *fm)
{
	if (fm->fc->conn_init) {
		FUSE_ARGS(args);

		args.opcode = FUSE_DESTROY;
		args.force = true;
		args.nocreds = true;
		fuse_simple_request(fm, &args);
	}
}