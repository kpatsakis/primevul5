  Impl(const RGWProcessEnv& env, RGWFrontendConfig* conf,
       rgw::dmclock::SchedulerCtx& sched_ctx)
    : AsioFrontend(env, conf, sched_ctx) {}