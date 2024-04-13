gst_flxdec_change_state (GstElement * element, GstStateChange transition)
{
  GstFlxDec *flxdec;
  GstStateChangeReturn ret;

  flxdec = GST_FLXDEC (element);

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_adapter_clear (flxdec->adapter);
      flxdec->state = GST_FLXDEC_READ_HEADER;
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      if (flxdec->frame_data) {
        g_free (flxdec->frame_data);
        flxdec->frame_data = NULL;
      }
      if (flxdec->delta_data) {
        g_free (flxdec->delta_data);
        flxdec->delta_data = NULL;
      }
      if (flxdec->converter) {
        flx_colorspace_converter_destroy (flxdec->converter);
        flxdec->converter = NULL;
      }
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }
  return ret;
}