ly_add_loaded_plugin(char *name)
{
    loaded_plugins = ly_realloc(loaded_plugins, (loaded_plugins_count + 2) * sizeof *loaded_plugins);
    LY_CHECK_ERR_RETURN(!loaded_plugins, free(name); LOGMEM(NULL), );
    ++loaded_plugins_count;

    loaded_plugins[loaded_plugins_count - 1] = name;
    loaded_plugins[loaded_plugins_count] = NULL;
}