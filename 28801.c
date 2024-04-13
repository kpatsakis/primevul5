tar_sparse_scan (struct tar_sparse_file *file, enum sparse_scan_state state,
		 void *block)
{
  if (file->optab->scan_block)
    return file->optab->scan_block (file, state, block);
  return true;
}
