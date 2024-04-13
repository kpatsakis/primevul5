plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "flxdec",
      GST_RANK_PRIMARY, GST_TYPE_FLXDEC);
}