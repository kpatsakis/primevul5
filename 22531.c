ciEnv::ciEnv(Arena* arena) : _ciEnv_arena(mtCompiler) {
  ASSERT_IN_VM;

  // Set up ciEnv::current immediately, for the sake of ciObjectFactory, etc.
  CompilerThread* current_thread = CompilerThread::current();
  assert(current_thread->env() == NULL, "must be");
  current_thread->set_env(this);
  assert(ciEnv::current() == this, "sanity");

  _oop_recorder = NULL;
  _debug_info = NULL;
  _dependencies = NULL;
  _failure_reason = NULL;
  _inc_decompile_count_on_failure = true;
  _compilable = MethodCompilable_never;
  _break_at_compile = false;
  _compiler_data = NULL;
#ifndef PRODUCT
  assert(firstEnv, "must be first");
  firstEnv = false;
#endif /* !PRODUCT */

  _num_inlined_bytecodes = 0;
  _task = NULL;
  _log = NULL;

  // Temporary buffer for creating symbols and such.
  _name_buffer = NULL;
  _name_buffer_len = 0;

  _arena   = arena;
  _factory = new (_arena) ciObjectFactory(_arena, 128);

  // Preload commonly referenced system ciObjects.

  // During VM initialization, these instances have not yet been created.
  // Assertions ensure that these instances are not accessed before
  // their initialization.

  assert(Universe::is_fully_initialized(), "must be");

  _NullPointerException_instance = NULL;
  _ArithmeticException_instance = NULL;
  _ArrayIndexOutOfBoundsException_instance = NULL;
  _ArrayStoreException_instance = NULL;
  _ClassCastException_instance = NULL;
  _the_null_string = NULL;
  _the_min_jint_string = NULL;

  _jvmti_redefinition_count = 0;
  _jvmti_can_hotswap_or_post_breakpoint = false;
  _jvmti_can_access_local_variables = false;
  _jvmti_can_post_on_exceptions = false;
  _jvmti_can_pop_frame = false;
}