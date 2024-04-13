void cpu_address_space_init(CPUState *cpu, int asidx,
                            const char *prefix, MemoryRegion *mr)
{
    CPUAddressSpace *newas;
    AddressSpace *as = g_new0(AddressSpace, 1);
    char *as_name;

    assert(mr);
    as_name = g_strdup_printf("%s-%d", prefix, cpu->cpu_index);
    address_space_init(as, mr, as_name);
    g_free(as_name);

    /* Target code should have set num_ases before calling us */
    assert(asidx < cpu->num_ases);

    if (asidx == 0) {
        /* address space 0 gets the convenience alias */
        cpu->as = as;
    }

    /* KVM cannot currently support multiple address spaces. */
    assert(asidx == 0 || !kvm_enabled());

    if (!cpu->cpu_ases) {
        cpu->cpu_ases = g_new0(CPUAddressSpace, cpu->num_ases);
    }

    newas = &cpu->cpu_ases[asidx];
    newas->cpu = cpu;
    newas->as = as;
    if (tcg_enabled()) {
        newas->tcg_as_listener.log_global_after_sync = tcg_log_global_after_sync;
        newas->tcg_as_listener.commit = tcg_commit;
        newas->tcg_as_listener.name = "tcg";
        memory_listener_register(&newas->tcg_as_listener, as);
    }
}