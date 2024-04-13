static inline void gprinter_put_minor(int minor)
{
	ida_simple_remove(&printer_ida, minor);
}