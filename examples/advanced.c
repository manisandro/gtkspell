/* vim: set ts=4 sw=4 wm=5 : */

/* This example demonstrates detaching and reattaching GtkSpell.
 * Also, we try using German instead of English. */

#include <gtk/gtk.h>
#include <gtkspell/gtkspell.h>

const char *langs[] = { "en_US", "de_DE", "ja_JP", NULL };

GtkWidget *window, *languagelist, *attached, *view;

static void
report_gtkspell_error(const char *err) {
	GtkWidget *dlg;
	dlg = gtk_message_dialog_new(
			GTK_WINDOW(window),
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,
			"GtkSpell error: %s", err);
	gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(dlg);
}

static void
attach_cb() {
	GtkSpell *spell;
	GError *error = NULL;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(attached))) {
		int lang;

		lang = gtk_option_menu_get_history(GTK_OPTION_MENU(languagelist));

		spell = gtkspell_new_attach(GTK_TEXT_VIEW(view), langs[lang], &error);

		if (spell == NULL) {
			report_gtkspell_error(error->message);
			g_error_free(error);
		}
	} else {
		gtkspell_detach(gtkspell_get_from_text_view(GTK_TEXT_VIEW(view)));
	}
}

static void
setlang_cb() {
	GtkSpell *spell;
	int lang;
	GError *error = NULL;

	spell = gtkspell_get_from_text_view(GTK_TEXT_VIEW(view));
	if (spell == NULL)
		return;

	lang = gtk_option_menu_get_history(GTK_OPTION_MENU(languagelist));
	if (!gtkspell_set_language(spell, langs[lang], &error)) {
		report_gtkspell_error(error->message);
		g_error_free(error);
	}
}

static void
build_languagelist() {
	int i;
	GtkWidget *menu, *mi;

	languagelist = gtk_option_menu_new();
	menu = gtk_menu_new();
	for (i = 0; langs[i] != NULL; i++) {
		mi = gtk_menu_item_new_with_label(langs[i]);
		gtk_widget_show(mi);
		g_signal_connect(G_OBJECT(mi), "activate",
				G_CALLBACK(setlang_cb), NULL);
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);
	}
	gtk_option_menu_set_menu(GTK_OPTION_MENU(languagelist), menu);
}

int
main(int argc, char* argv[]) {
	GtkWidget *box, *hbox, *scroll;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	view = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD);

	scroll = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
			GTK_POLICY_AUTOMATIC,
			GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),
			GTK_SHADOW_IN);
	gtk_container_add(GTK_CONTAINER(scroll), view);

	build_languagelist();

	hbox = gtk_hbox_new(FALSE, 5);
	attached = gtk_toggle_button_new_with_label("Attached");
	g_signal_connect(G_OBJECT(attached), "toggled",
			G_CALLBACK(attach_cb), NULL);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(attached), TRUE);
	gtk_box_pack_start(GTK_BOX(hbox), attached, FALSE, FALSE, 0);

	gtk_box_pack_end(GTK_BOX(hbox), languagelist, FALSE, FALSE, 0);

	box = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(box);

	gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
	gtk_window_set_title(GTK_WINDOW(window), "\"Advanced\" GtkSpell Demonstration");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	g_signal_connect(G_OBJECT(window), "delete-event",
			G_CALLBACK(gtk_main_quit), NULL);
	gtk_container_add(GTK_CONTAINER(window), box);

	gtk_widget_show(window);
	gtk_main();

	return 0;
}
