render_pixbuf_size_prepared_cb (GdkPixbufLoader *loader,
				gint             width,
				gint             height,
				gpointer         data)
{
	double *scale = data;
	int w = (width  * (*scale) + 0.5);
	int h = (height * (*scale) + 0.5);

	gdk_pixbuf_loader_set_size (loader, w, h);
}