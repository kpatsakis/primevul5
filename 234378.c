printerr_command()
{
    FILE *save_print_out = print_out;

    print_out = stderr;
    print_command();
    print_out = save_print_out;
}