static void print_lat_help_header(struct seq_file *m)
{
	seq_puts(m, "#                  _------=> CPU#            \n"
		    "#                 / _-----=> irqs-off        \n"
		    "#                | / _----=> need-resched    \n"
		    "#                || / _---=> hardirq/softirq \n"
		    "#                ||| / _--=> preempt-depth   \n"
		    "#                |||| /     delay            \n"
		    "#  cmd     pid   ||||| time  |   caller      \n"
		    "#     \\   /      |||||  \\    |   /         \n");
}