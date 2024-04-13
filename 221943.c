bool fuse_invalid_attr(struct fuse_attr *attr)
{
	return !fuse_valid_type(attr->mode) ||
		attr->size > LLONG_MAX;
}