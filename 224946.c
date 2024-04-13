}
EXPORT_SYMBOL_GPL(iscsi_unregister_transport);

void iscsi_dbg_trace(void (*trace)(struct device *dev, struct va_format *),
		     struct device *dev, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;