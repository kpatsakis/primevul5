sudoers_policy_invalidate(int unlinkit)
{
    debug_decl(sudoers_policy_invalidate, SUDOERS_DEBUG_PLUGIN);

    user_cmnd = "kill";
    /* XXX - plugin API should support a return value for fatal errors. */
    timestamp_remove(unlinkit);
    sudoers_cleanup();

    debug_return;
}