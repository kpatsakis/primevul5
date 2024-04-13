static sljit_uw * allocate_read_only_data(compiler_common *common, sljit_uw size)
{
DEFINE_COMPILER;
sljit_uw *result;

if (SLJIT_UNLIKELY(sljit_get_compiler_error(compiler)))
  return NULL;

result = (sljit_uw *)SLJIT_MALLOC(size + sizeof(sljit_uw), compiler->allocator_data);
if (SLJIT_UNLIKELY(result == NULL))
  {
  sljit_set_compiler_memory_error(compiler);
  return NULL;
  }

*(void**)result = common->read_only_data_head;
common->read_only_data_head = (void *)result;
return result + 1;
}