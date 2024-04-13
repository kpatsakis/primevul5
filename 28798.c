tar_sparse_dump_region (struct tar_sparse_file *file, size_t i)
{
  if (file->optab->dump_region)
    return file->optab->dump_region (file, i);
  return false;
}
