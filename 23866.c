save_dialog (void)
{
  GtkWidget *dialog;
  GtkWidget *label;
  GtkWidget *toggle;
  GtkWidget *combo;
  GtkWidget *vbox;
  GtkWidget *hbox;
  gboolean   run;

  dialog = gimp_export_dialog_new (_("TGA"), PLUG_IN_BINARY, SAVE_PROC);

  vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 12);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 12);
  gtk_box_pack_start (GTK_BOX (gimp_export_dialog_get_content_area (dialog)),
                      vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);

  /*  rle  */
  toggle = gtk_check_button_new_with_mnemonic (_("_RLE compression"));
  gtk_box_pack_start (GTK_BOX (vbox), toggle, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (toggle), tsvals.rle);
  gtk_widget_show (toggle);

  g_signal_connect (toggle, "toggled",
                    G_CALLBACK (gimp_toggle_button_update),
                    &tsvals.rle);

  /*  origin  */
  hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  label = gtk_label_new_with_mnemonic (_("Or_igin:"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  combo = gimp_int_combo_box_new (_("Bottom left"), ORIGIN_BOTTOM_LEFT,
                                  _("Top left"),    ORIGIN_TOP_LEFT,
                                  NULL);
  gtk_box_pack_start (GTK_BOX (hbox), combo, TRUE, TRUE, 0);
  gtk_widget_show (combo);

  gtk_label_set_mnemonic_widget (GTK_LABEL (label), combo);

  gimp_int_combo_box_connect (GIMP_INT_COMBO_BOX (combo),
                              tsvals.origin,
                              G_CALLBACK (gimp_int_combo_box_get_active),
                              &tsvals.origin);

  gtk_widget_show (dialog);

  run = (gimp_dialog_run (GIMP_DIALOG (dialog)) == GTK_RESPONSE_OK);

  gtk_widget_destroy (dialog);

  return run;
}