int RGWFormPost::get_data(ceph::bufferlist& bl, bool& again)
{
  bool boundary;

  int r = read_data(bl, s->cct->_conf->rgw_max_chunk_size,
                    boundary, stream_done);
  if (r < 0) {
    return r;
  }

  /* Tell RGWPostObj::execute() that it has some data to put. */
  again = !boundary;

  return bl.length();
}