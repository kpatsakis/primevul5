bool ciEnv::jvmti_state_changed() const {
  // Some classes were redefined
  if (_jvmti_redefinition_count != JvmtiExport::redefinition_count()) {
    return true;
  }

  if (!_jvmti_can_access_local_variables &&
      JvmtiExport::can_access_local_variables()) {
    return true;
  }
  if (!_jvmti_can_hotswap_or_post_breakpoint &&
      JvmtiExport::can_hotswap_or_post_breakpoint()) {
    return true;
  }
  if (!_jvmti_can_post_on_exceptions &&
      JvmtiExport::can_post_on_exceptions()) {
    return true;
  }
  if (!_jvmti_can_pop_frame &&
      JvmtiExport::can_pop_frame()) {
    return true;
  }
  if (!_jvmti_can_get_owned_monitor_info &&
      JvmtiExport::can_get_owned_monitor_info()) {
    return true;
  }
  if (!_jvmti_can_walk_any_space &&
      JvmtiExport::can_walk_any_space()) {
    return true;
  }

  return false;
}