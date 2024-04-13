read_in_header (struct cpio_file_stat *file_hdr, int in_des)
{
  union {
    char str[6];
    unsigned short num;
    struct old_cpio_header old_header;
  } magic;
  long bytes_skipped = 0;	/* Bytes of junk found before magic number.  */

  /* Search for a valid magic number.  */

  if (archive_format == arf_unknown)
    {
      union
      {
	char s[512];
	unsigned short us;
      }	tmpbuf;
      int check_tar;
      int peeked_bytes;

      while (archive_format == arf_unknown)
	{
	  peeked_bytes = tape_buffered_peek (tmpbuf.s, in_des, 512);
	  if (peeked_bytes < 6)
	    error (PAXEXIT_FAILURE, 0, _("premature end of archive"));

	  if (!strncmp (tmpbuf.s, "070701", 6))
	    archive_format = arf_newascii;
	  else if (!strncmp (tmpbuf.s, "070707", 6))
	    archive_format = arf_oldascii;
	  else if (!strncmp (tmpbuf.s, "070702", 6))
	    {
	      archive_format = arf_crcascii;
	      crc_i_flag = true;
	    }
	  else if (tmpbuf.us == 070707
		   || tmpbuf.us == swab_short ((unsigned short) 070707))
	    archive_format = arf_binary;
	  else if (peeked_bytes >= 512
		   && (check_tar = is_tar_header (tmpbuf.s)))
	    {
	      if (check_tar == 2)
		archive_format = arf_ustar;
	      else
		archive_format = arf_tar;
	    }
	  else
	    {
	      tape_buffered_read (tmpbuf.s, in_des, 1L);
	      ++bytes_skipped;
	    }
	}
    }

  if (archive_format == arf_tar || archive_format == arf_ustar)
    {
      if (append_flag)
	last_header_start = input_bytes - io_block_size +
	  (in_buff - input_buffer);
      if (bytes_skipped > 0)
	warn_junk_bytes (bytes_skipped);

      read_in_tar_header (file_hdr, in_des);
      return;
    }

  file_hdr->c_tar_linkname = NULL;

  tape_buffered_read (magic.str, in_des, sizeof (magic.str));
  while (1)
    {
      if (append_flag)
	last_header_start = input_bytes - io_block_size
	  + (in_buff - input_buffer) - 6;
      if (archive_format == arf_newascii
	  && !strncmp (magic.str, "070701", 6))
	{
	  if (bytes_skipped > 0)
	    warn_junk_bytes (bytes_skipped);
	  file_hdr->c_magic = 070701;
	  read_in_new_ascii (file_hdr, in_des);
	  break;
	}
      if (archive_format == arf_crcascii
	  && !strncmp (magic.str, "070702", 6))
	{
	  if (bytes_skipped > 0)
	    warn_junk_bytes (bytes_skipped);
	  file_hdr->c_magic = 070702;
	  read_in_new_ascii (file_hdr, in_des);
	  break;
	}
      if ( (archive_format == arf_oldascii || archive_format == arf_hpoldascii)
	  && !strncmp (magic.str, "070707", 6))
	{
	  if (bytes_skipped > 0)
	    warn_junk_bytes (bytes_skipped);
	  file_hdr->c_magic = 070707;
	  read_in_old_ascii (file_hdr, in_des);
	  break;
	}
      if ( (archive_format == arf_binary || archive_format == arf_hpbinary)
	  && (magic.num == 070707
	      || magic.num == swab_short ((unsigned short) 070707)))
	{
	  /* Having to skip 1 byte because of word alignment is normal.  */
	  if (bytes_skipped > 0)
	    warn_junk_bytes (bytes_skipped);
	  file_hdr->c_magic = 070707;
	  read_in_binary (file_hdr, &magic.old_header, in_des);
	  break;
	}
      bytes_skipped++;
      memmove (magic.str, magic.str + 1, sizeof (magic.str) - 1);
      tape_buffered_read (magic.str + sizeof (magic.str) - 1, in_des, 1L);
    }
}