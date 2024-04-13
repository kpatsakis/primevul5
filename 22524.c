void ciEnv::register_method(ciMethod* target,
                            int entry_bci,
                            CodeOffsets* offsets,
                            int orig_pc_offset,
                            CodeBuffer* code_buffer,
                            int frame_words,
                            OopMapSet* oop_map_set,
                            ExceptionHandlerTable* handler_table,
                            ImplicitExceptionTable* inc_table,
                            AbstractCompiler* compiler,
                            bool has_unsafe_access,
                            bool has_wide_vectors,
                            RTMState  rtm_state,
                            const GrowableArrayView<RuntimeStub*>& native_invokers) {
  VM_ENTRY_MARK;
  nmethod* nm = NULL;
  {
    methodHandle method(THREAD, target->get_Method());

    // We require method counters to store some method state (max compilation levels) required by the compilation policy.
    if (method->get_method_counters(THREAD) == NULL) {
      record_failure("can't create method counters");
      // All buffers in the CodeBuffer are allocated in the CodeCache.
      // If the code buffer is created on each compile attempt
      // as in C2, then it must be freed.
      code_buffer->free_blob();
      return;
    }

    // To prevent compile queue updates.
    MutexLocker locker(THREAD, MethodCompileQueue_lock);

    // Prevent SystemDictionary::add_to_hierarchy from running
    // and invalidating our dependencies until we install this method.
    // No safepoints are allowed. Otherwise, class redefinition can occur in between.
    MutexLocker ml(Compile_lock);
    NoSafepointVerifier nsv;

    // Change in Jvmti state may invalidate compilation.
    if (!failing() && jvmti_state_changed()) {
      record_failure("Jvmti state change invalidated dependencies");
    }

    // Change in DTrace flags may invalidate compilation.
    if (!failing() &&
        ( (!dtrace_extended_probes() && ExtendedDTraceProbes) ||
          (!dtrace_method_probes() && DTraceMethodProbes) ||
          (!dtrace_alloc_probes() && DTraceAllocProbes) )) {
      record_failure("DTrace flags change invalidated dependencies");
    }

    if (!failing() && target->needs_clinit_barrier() &&
        target->holder()->is_in_error_state()) {
      record_failure("method holder is in error state");
    }

    if (!failing()) {
      if (log() != NULL) {
        // Log the dependencies which this compilation declares.
        dependencies()->log_all_dependencies();
      }

      // Encode the dependencies now, so we can check them right away.
      dependencies()->encode_content_bytes();

      // Check for {class loads, evolution, breakpoints, ...} during compilation
      validate_compile_task_dependencies(target);
    }
#if INCLUDE_RTM_OPT
    if (!failing() && (rtm_state != NoRTM) &&
        (method()->method_data() != NULL) &&
        (method()->method_data()->rtm_state() != rtm_state)) {
      // Preemptive decompile if rtm state was changed.
      record_failure("RTM state change invalidated rtm code");
    }
#endif

    if (failing()) {
      // While not a true deoptimization, it is a preemptive decompile.
      MethodData* mdo = method()->method_data();
      if (mdo != NULL && _inc_decompile_count_on_failure) {
        mdo->inc_decompile_count();
      }

      // All buffers in the CodeBuffer are allocated in the CodeCache.
      // If the code buffer is created on each compile attempt
      // as in C2, then it must be freed.
      code_buffer->free_blob();
      return;
    }

    assert(offsets->value(CodeOffsets::Deopt) != -1, "must have deopt entry");
    assert(offsets->value(CodeOffsets::Exceptions) != -1, "must have exception entry");

    nm =  nmethod::new_nmethod(method,
                               compile_id(),
                               entry_bci,
                               offsets,
                               orig_pc_offset,
                               debug_info(), dependencies(), code_buffer,
                               frame_words, oop_map_set,
                               handler_table, inc_table,
                               compiler, task()->comp_level(),
                               native_invokers);

    // Free codeBlobs
    code_buffer->free_blob();

    if (nm != NULL) {
      nm->set_has_unsafe_access(has_unsafe_access);
      nm->set_has_wide_vectors(has_wide_vectors);
#if INCLUDE_RTM_OPT
      nm->set_rtm_state(rtm_state);
#endif

      // Record successful registration.
      // (Put nm into the task handle *before* publishing to the Java heap.)
      if (task() != NULL) {
        task()->set_code(nm);
      }

      if (entry_bci == InvocationEntryBci) {
        if (TieredCompilation) {
          // If there is an old version we're done with it
          CompiledMethod* old = method->code();
          if (TraceMethodReplacement && old != NULL) {
            ResourceMark rm;
            char *method_name = method->name_and_sig_as_C_string();
            tty->print_cr("Replacing method %s", method_name);
          }
          if (old != NULL) {
            old->make_not_used();
          }
        }

        LogTarget(Info, nmethod, install) lt;
        if (lt.is_enabled()) {
          ResourceMark rm;
          char *method_name = method->name_and_sig_as_C_string();
          lt.print("Installing method (%d) %s ",
                    task()->comp_level(), method_name);
        }
        // Allow the code to be executed
        MutexLocker ml(CompiledMethod_lock, Mutex::_no_safepoint_check_flag);
        if (nm->make_in_use()) {
          method->set_code(method, nm);
        }
      } else {
        LogTarget(Info, nmethod, install) lt;
        if (lt.is_enabled()) {
          ResourceMark rm;
          char *method_name = method->name_and_sig_as_C_string();
          lt.print("Installing osr method (%d) %s @ %d",
                    task()->comp_level(), method_name, entry_bci);
        }
        MutexLocker ml(CompiledMethod_lock, Mutex::_no_safepoint_check_flag);
        if (nm->make_in_use()) {
          method->method_holder()->add_osr_nmethod(nm);
        }
      }
    }
  }  // safepoints are allowed again

  if (nm != NULL) {
    // JVMTI -- compiled method notification (must be done outside lock)
    nm->post_compiled_method_load_event();
  } else {
    // The CodeCache is full.
    record_failure("code cache is full");
  }
}