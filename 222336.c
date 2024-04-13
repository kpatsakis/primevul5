void input_free_minor(unsigned int minor)
{
	ida_simple_remove(&input_ida, minor);
}