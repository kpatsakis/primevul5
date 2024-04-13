bool ciEnv::cache_jvmti_state() {
  VM_ENTRY_MARK;
  // Get Jvmti capabilities under lock to get consistant values.
  MutexLocker mu(JvmtiThreadState_lock);
  _jvmti_redefinition_count             = JvmtiExport::redefinition_count();
  _jvmti_can_hotswap_or_post_breakpoint = JvmtiExport::can_hotswap_or_post_breakpoint();
  _jvmti_can_access_local_variables     = JvmtiExport::can_access_local_variables();
  _jvmti_can_post_on_exceptions         = JvmtiExport::can_post_on_exceptions();
  _jvmti_can_pop_frame                  = JvmtiExport::can_pop_frame();
  _jvmti_can_get_owned_monitor_info     = JvmtiExport::can_get_owned_monitor_info();
  _jvmti_can_walk_any_space             = JvmtiExport::can_walk_any_space();
  return _task != NULL && _task->method()->is_old();
}