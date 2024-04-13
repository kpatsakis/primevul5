log_allocator (void *pointer,
               size_t size)
{
	void *result = realloc (pointer, (size_t)size);
	return_val_if_fail (!size || result != NULL, NULL);
	return result;
}