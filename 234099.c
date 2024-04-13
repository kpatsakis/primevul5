inode_val_hasher (const void *val, size_t n_buckets)
{
  const struct inode_val *ival = val;
  return ival->inode % n_buckets;
}