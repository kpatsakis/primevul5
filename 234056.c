tape_pad_output (int out_file_des, int offset)
{
  size_t pad;

  if (archive_format == arf_newascii || archive_format == arf_crcascii)
    pad = (4 - (offset % 4)) % 4;
  else if (archive_format == arf_tar || archive_format == arf_ustar)
    pad = (512 - (offset % 512)) % 512;
  else if (archive_format != arf_oldascii && archive_format != arf_hpoldascii)
    pad = (2 - (offset % 2)) % 2;
  else
    pad = 0;

  if (pad != 0)
    write_nuls_to_file (pad, out_file_des, tape_buffered_write);
}