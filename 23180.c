gst_flxdec_sink_event_handler (GstPad * pad, GstObject * parent,
    GstEvent * event)
{
  GstFlxDec *flxdec;
  gboolean ret;

  flxdec = GST_FLXDEC (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEGMENT:
    {
      GstSegment segment;

      gst_event_copy_segment (event, &segment);
      if (segment.format != GST_FORMAT_TIME) {
        GST_DEBUG_OBJECT (flxdec, "generating TIME segment");
        gst_segment_init (&segment, GST_FORMAT_TIME);
        gst_event_unref (event);
        event = gst_event_new_segment (&segment);
      }
      /* fall-through */
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }

  return ret;
}