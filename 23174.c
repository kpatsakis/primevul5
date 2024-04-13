gst_flxdec_init (GstFlxDec * flxdec)
{
  flxdec->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_element_add_pad (GST_ELEMENT (flxdec), flxdec->sinkpad);
  gst_pad_set_chain_function (flxdec->sinkpad,
      GST_DEBUG_FUNCPTR (gst_flxdec_chain));
  gst_pad_set_event_function (flxdec->sinkpad,
      GST_DEBUG_FUNCPTR (gst_flxdec_sink_event_handler));

  flxdec->srcpad = gst_pad_new_from_static_template (&src_video_factory, "src");
  gst_element_add_pad (GST_ELEMENT (flxdec), flxdec->srcpad);
  gst_pad_set_query_function (flxdec->srcpad,
      GST_DEBUG_FUNCPTR (gst_flxdec_src_query_handler));

  gst_pad_use_fixed_caps (flxdec->srcpad);

  flxdec->adapter = gst_adapter_new ();
}