gst_flxdec_src_query_handler (GstPad * pad, GstObject * parent,
    GstQuery * query)
{
  GstFlxDec *flxdec = (GstFlxDec *) parent;
  gboolean ret = FALSE;

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_DURATION:
    {
      GstFormat format;

      gst_query_parse_duration (query, &format, NULL);

      if (format != GST_FORMAT_TIME)
        goto done;

      gst_query_set_duration (query, format, flxdec->duration);

      ret = TRUE;
    }
    default:
      break;
  }
done:
  if (!ret)
    ret = gst_pad_query_default (pad, parent, query);

  return ret;
}