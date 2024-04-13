void ciEnv::report_failure(const char* reason) {
  EventCompilationFailure event;
  if (event.should_commit()) {
    CompilerEvent::CompilationFailureEvent::post(event, compile_id(), reason);
  }
}