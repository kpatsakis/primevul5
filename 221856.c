static int fuse_parse_param(struct fs_context *fc, struct fs_parameter *param)
{
	struct fs_parse_result result;
	struct fuse_fs_context *ctx = fc->fs_private;
	int opt;

	if (fc->purpose == FS_CONTEXT_FOR_RECONFIGURE) {
		/*
		 * Ignore options coming from mount(MS_REMOUNT) for backward
		 * compatibility.
		 */
		if (fc->oldapi)
			return 0;

		return invalfc(fc, "No changes allowed in reconfigure");
	}

	opt = fs_parse(fc, fuse_fs_parameters, param, &result);
	if (opt < 0)
		return opt;

	switch (opt) {
	case OPT_SOURCE:
		if (fc->source)
			return invalfc(fc, "Multiple sources specified");
		fc->source = param->string;
		param->string = NULL;
		break;

	case OPT_SUBTYPE:
		if (ctx->subtype)
			return invalfc(fc, "Multiple subtypes specified");
		ctx->subtype = param->string;
		param->string = NULL;
		return 0;

	case OPT_FD:
		ctx->fd = result.uint_32;
		ctx->fd_present = true;
		break;

	case OPT_ROOTMODE:
		if (!fuse_valid_type(result.uint_32))
			return invalfc(fc, "Invalid rootmode");
		ctx->rootmode = result.uint_32;
		ctx->rootmode_present = true;
		break;

	case OPT_USER_ID:
		ctx->user_id = make_kuid(fc->user_ns, result.uint_32);
		if (!uid_valid(ctx->user_id))
			return invalfc(fc, "Invalid user_id");
		ctx->user_id_present = true;
		break;

	case OPT_GROUP_ID:
		ctx->group_id = make_kgid(fc->user_ns, result.uint_32);
		if (!gid_valid(ctx->group_id))
			return invalfc(fc, "Invalid group_id");
		ctx->group_id_present = true;
		break;

	case OPT_DEFAULT_PERMISSIONS:
		ctx->default_permissions = true;
		break;

	case OPT_ALLOW_OTHER:
		ctx->allow_other = true;
		break;

	case OPT_MAX_READ:
		ctx->max_read = result.uint_32;
		break;

	case OPT_BLKSIZE:
		if (!ctx->is_bdev)
			return invalfc(fc, "blksize only supported for fuseblk");
		ctx->blksize = result.uint_32;
		break;

	default:
		return -EINVAL;
	}

	return 0;
}