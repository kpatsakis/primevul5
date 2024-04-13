write_out_header (struct cpio_file_stat *file_hdr, int out_des)
{
  dev_t dev;
  dev_t rdev;
  
  switch (archive_format)
    {
    case arf_newascii:
      return write_out_new_ascii_header ("070701", file_hdr, out_des);
      
    case arf_crcascii:
      return write_out_new_ascii_header ("070702", file_hdr, out_des);
      
    case arf_oldascii:
      return write_out_old_ascii_header (makedev (file_hdr->c_dev_maj,
						  file_hdr->c_dev_min),
					 makedev (file_hdr->c_rdev_maj,
						  file_hdr->c_rdev_min),
					 file_hdr, out_des);
      
    case arf_hpoldascii:
      hp_compute_dev (file_hdr, &dev, &rdev);
      return write_out_old_ascii_header (dev, rdev, file_hdr, out_des);
      
    case arf_tar:
    case arf_ustar:
      if (is_tar_filename_too_long (file_hdr->c_name))
	{
	  error (0, 0, _("%s: file name too long"), file_hdr->c_name);
	  return 1;
	}
      return write_out_tar_header (file_hdr, out_des);

    case arf_binary:
      return write_out_binary_header (makedev (file_hdr->c_rdev_maj,
					       file_hdr->c_rdev_min),
				      file_hdr, out_des);

    case arf_hpbinary:
      hp_compute_dev (file_hdr, &dev, &rdev);
      /* FIXME: dev ignored. Should it be? */
      return write_out_binary_header (rdev, file_hdr, out_des);

    default:
      abort ();
    }
}