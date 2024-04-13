print_show_output()
{
    if (print_out_name)
	return print_out_name;
    if (print_out == stdout)
	return "<stdout>";
    if (!print_out || print_out == stderr || !print_out_name)
	return "<stderr>";
    return print_out_name;
}