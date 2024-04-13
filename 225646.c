auto make_stack_allocator() {
  return boost::context::protected_fixedsize_stack{512*1024};
}