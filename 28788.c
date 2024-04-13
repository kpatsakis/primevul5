sparse_add_map (struct tar_stat_info *st, struct sp_array const *sp)
{
  struct sp_array *sparse_map = st->sparse_map;
  size_t avail = st->sparse_map_avail;
  if (avail == st->sparse_map_size)
    st->sparse_map = sparse_map =
      x2nrealloc (sparse_map, &st->sparse_map_size, sizeof *sparse_map);
  sparse_map[avail] = *sp;
  st->sparse_map_avail = avail + 1;
}
