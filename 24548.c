int ha_maria::rnd_end()
{
  ds_mrr.dsmrr_close();
  /* Safe to call even if we don't have started a scan */
  maria_scan_end(file);
  return 0;
}