int ciEnv::comp_level() {
  if (task() == NULL)  return CompilationPolicy::highest_compile_level();
  return task()->comp_level();
}