int ha_maria::remember_rnd_pos()
{
  register_handler(file);
  return (*file->s->scan_remember_pos)(file, &remember_pos);
}