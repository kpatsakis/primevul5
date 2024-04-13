write_nuls_to_file (off_t num_bytes, int out_des, 
                    void (*writer) (char *in_buf, int out_des, off_t num_bytes))
{
  off_t	blocks;
  off_t	extra_bytes;
  off_t	i;
  static char zeros_512[512];
  
  blocks = num_bytes / sizeof zeros_512;
  extra_bytes = num_bytes % sizeof zeros_512;
  for (i = 0; i < blocks; ++i)
    writer (zeros_512, out_des, sizeof zeros_512);
  if (extra_bytes)
    writer (zeros_512, out_des, extra_bytes);
}