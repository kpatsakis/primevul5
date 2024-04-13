check_data_region (struct tar_sparse_file *file, size_t i)
{
  off_t size_left;

  if (!lseek_or_error (file, file->stat_info->sparse_map[i].offset))
    return false;
 			     rdsize);
 	  return false;
 	}
