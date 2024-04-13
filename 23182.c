gst_flxdec_class_init (GstFlxDecClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->dispose = (GObjectFinalizeFunc) gst_flxdec_dispose;

  GST_DEBUG_CATEGORY_INIT (flxdec_debug, "flxdec", 0, "FLX video decoder");

  gstelement_class->change_state = GST_DEBUG_FUNCPTR (gst_flxdec_change_state);

  gst_element_class_set_static_metadata (gstelement_class, "FLX video decoder",
      "Codec/Decoder/Video",
      "FLC/FLI/FLX video decoder",
      "Sepp Wijnands <mrrazz@garbage-coderz.net>, Zeeshan Ali <zeenix@gmail.com>");
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_video_factory));
}