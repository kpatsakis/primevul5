  AsioFrontend(const RGWProcessEnv& env, RGWFrontendConfig* conf,
	       dmc::SchedulerCtx& sched_ctx)
    : env(env), conf(conf), pause_mutex(context.get_executor())
  {
    auto sched_t = dmc::get_scheduler_t(ctx());
    switch(sched_t){
    case dmc::scheduler_t::dmclock:
      scheduler.reset(new dmc::AsyncScheduler(ctx(),
                                              context,
                                              std::ref(sched_ctx.get_dmc_client_counters()),
                                              sched_ctx.get_dmc_client_config(),
                                              *sched_ctx.get_dmc_client_config(),
                                              dmc::AtLimit::Reject));
      break;
    case dmc::scheduler_t::none:
      lderr(ctx()) << "Got invalid scheduler type for beast, defaulting to throttler" << dendl;
      [[fallthrough]];
    case dmc::scheduler_t::throttler:
      scheduler.reset(new dmc::SimpleThrottler(ctx()));

    }
  }