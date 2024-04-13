gst_flxdec_dispose (GstFlxDec * flxdec)
{
  if (flxdec->adapter) {
    g_object_unref (flxdec->adapter);
    flxdec->adapter = NULL;
  }

  G_OBJECT_CLASS (parent_class)->dispose ((GObject *) flxdec);
}