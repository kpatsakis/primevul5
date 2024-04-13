void ocall_throw(const char *message) {
  JNIEnv* env;
  jvm->AttachCurrentThread((void**) &env, NULL);
  jclass exception = env->FindClass("edu/berkeley/cs/rise/opaque/OpaqueException");
  env->ThrowNew(exception, message);
}