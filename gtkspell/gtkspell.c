/* gtkspell - a spell-checking addon for GTK's TextView widget
 * Copyright (c) 2002 Evan Martin.
 */

/* vim: set ts=4 sw=4 wm=5 : */

#include <gtk/gtk.h>
#include <pspell/pspell.h>
#include "gtkspell.h"

const int debug = 0;
const int quiet = 0;

struct _GtkSpell {
	GtkTextView *view;
	GtkTextTag *tag_highlight;
	GtkTextMark *mark_insert;
	PspellManager *manager;
};

static void gtkspell_free(GtkSpell *spell);

#define GTKSPELL_OBJECT_KEY "gtkspell"

GQuark
gtkspell_error_quark(void) {
	static GQuark q = 0;
	if (q == 0)
		q = g_quark_from_static_string("gtkspell-error-quark");
	return q;
}

/* heuristic: 
 * if we're on an singlequote/apostrophe and
 * if the next letter is alphanumeric,
 * this is an apostrophe. */
static gboolean
gtkspell_text_iter_forward_word_end(GtkTextIter *i) {
	GtkTextIter iter;

	if (!gtk_text_iter_forward_word_end(i))
		return FALSE;

	if (gtk_text_iter_get_char(i) != '\'')
		return TRUE;

	iter = *i;
	if (gtk_text_iter_forward_char(&iter)) {
		if (g_unichar_isalpha(gtk_text_iter_get_char(&iter))) {
			return (gtk_text_iter_forward_word_end(i));
		}
	}

	return TRUE;
}

static gboolean
gtkspell_text_iter_backward_word_start(GtkTextIter *i) {
	GtkTextIter iter;

	if (!gtk_text_iter_backward_word_start(i))
		return FALSE;

	iter = *i;
	if (gtk_text_iter_backward_char(&iter)) {
		if (gtk_text_iter_get_char(&iter) == '\'') {
			if (gtk_text_iter_backward_char(&iter)) {
				if (g_unichar_isalpha(gtk_text_iter_get_char(&iter))) {
					return (gtk_text_iter_backward_word_start(i));
				}
			}
		}
	}

	return TRUE;
}

#define gtk_text_iter_backward_word_start gtkspell_text_iter_backward_word_start
#define gtk_text_iter_forward_word_end gtkspell_text_iter_forward_word_end

static void
check_word(GtkSpell *spell, GtkTextBuffer *buffer,
           GtkTextIter *start, GtkTextIter *end) {
	char *text;
	text = gtk_text_buffer_get_text(buffer, start, end, FALSE);
	if (debug) g_print("checking: %s\n", text);
	if (pspell_manager_check(spell->manager, text, -1) == FALSE)
		gtk_text_buffer_apply_tag(buffer, spell->tag_highlight, start, end);
	g_free(text);
}

static void
print_iter(char *name, GtkTextIter *iter) {
	g_print("%1s[%d%c%c%c] ", name, gtk_text_iter_get_offset(iter),
		gtk_text_iter_starts_word(iter) ? 's' : ' ',
		gtk_text_iter_inside_word(iter) ? 'i' : ' ',
		gtk_text_iter_ends_word(iter) ? 'e' : ' ');
}

static void
check_range(GtkSpell *spell, GtkTextBuffer *buffer,
            GtkTextIter start, GtkTextIter end) {
	/* we need to "split" on word boundaries.
	 * luckily, pango knows what "words" are 
	 * so we don't have to figure it out. */

	GtkTextIter wstart, wend;
	if (debug) {
		g_print("check_range: "); print_iter("s", &start); print_iter("e", &end); g_print(" -> ");
	}

	if (gtk_text_iter_inside_word(&end))
		gtk_text_iter_forward_word_end(&end);
	if (!gtk_text_iter_starts_word(&start)) {
		if (gtk_text_iter_inside_word(&start) || 
				gtk_text_iter_ends_word(&start)) {
			gtk_text_iter_backward_word_start(&start);
		} else {
			/* if we're neither at the beginning nor inside a word,
			 * me must be in some spaces.
			 * skip forward to the beginning of the next word. */
			//gtk_text_buffer_remove_tag(buffer, tag_highlight, &start, &end);
			if (gtk_text_iter_forward_word_end(&start))
				gtk_text_iter_backward_word_start(&start);
		}
	}
	gtk_text_buffer_remove_tag(buffer, spell->tag_highlight, &start, &end);

	if (debug) {print_iter("s", &start); print_iter("e", &end); g_print("\n");}

	wstart = start;
	while (gtk_text_iter_compare(&wstart, &end) < 0) {
		/* move wend to the end of the current word. */
		wend = wstart;
		gtk_text_iter_forward_word_end(&wend);

		check_word(spell, buffer, &wstart, &wend);

		/* now move wend to the beginning of the next word, */
		gtk_text_iter_forward_word_end(&wend);
		gtk_text_iter_backward_word_start(&wend);
		/* make sure we've actually advanced
		 * (we don't advance in some corner cases), */
		if (gtk_text_iter_equal(&wstart, &wend))
			break; /* we're done in these cases.. */
		/* and then pick this as the new next word beginning. */
		wstart = wend;
	}
}

/* insertion works like this:
 *  - before the text is inserted, we mark the position in the buffer.
 *  - after the text is inserted, we see where our mark is and use that and
 *    the current position to check the entire range of inserted text.
 *
 * this may be overkill for the common case (inserting one character). */

static void
insert_text_before(GtkTextBuffer *buffer, GtkTextIter *iter,
                   gchar *text, gint len, GtkSpell *spell) {
	gtk_text_buffer_move_mark(buffer, spell->mark_insert, iter);
}

static void
insert_text_after(GtkTextBuffer *buffer, GtkTextIter *iter,
                  gchar *text, gint len, GtkSpell *spell) {
	GtkTextIter start;

	/* we need to check a range of text. */
	gtk_text_buffer_get_iter_at_mark(buffer, &start, spell->mark_insert);
	check_range(spell, buffer, start, *iter);
}

/* deleting is more simple:  we're given the range of deleted text.
 * after deletion, the start and end iters should be at the same position
 * (because all of the text between them was deleted!).
 * this means we only really check the words immediately bounding the
 * deletion.
 */

static void
delete_range_after(GtkTextBuffer *buffer,
                   GtkTextIter *start, GtkTextIter *end, GtkSpell *spell) {
	check_range(spell, buffer, *start, *end);
}

static void
get_cur_word_extents(GtkTextBuffer *buffer,
                     GtkTextIter *start, GtkTextIter *end) {
	gtk_text_buffer_get_iter_at_mark(buffer, start, 
			gtk_text_buffer_get_insert(buffer));
	if (!gtk_text_iter_starts_word(start)) 
		gtk_text_iter_backward_word_start(start);
	*end = *start;
	if (gtk_text_iter_inside_word(end))
		gtk_text_iter_forward_word_end(end);
}

static void
add_to_dictionary(GtkWidget *menuitem, GtkSpell *spell) {
	char *word;
	GtkTextIter start, end;
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer(spell->view);

	get_cur_word_extents(buffer, &start, &end);
	word = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	
	pspell_manager_add_to_session(spell->manager, word, strlen(word));

	gtk_text_buffer_remove_tag(buffer, spell->tag_highlight, &start, &end);

	g_free(word);
}

static void
replace_word(GtkWidget *menuitem, GtkSpell *spell) {
	char *oldword;
	const char *newword;
	GtkTextIter start, end;
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer(spell->view);

	get_cur_word_extents(buffer, &start, &end);
	oldword = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	newword = gtk_label_get_text(GTK_LABEL(GTK_BIN(menuitem)->child));

	if (debug) {
		g_print("old word: '%s'\n", oldword);
		print_iter("s", &start); print_iter("e", &end);
		g_print("\nnew word: '%s'\n", newword);
	}

	gtk_text_buffer_delete(buffer, &start, &end);
	gtk_text_buffer_insert(buffer, &start, newword, -1);

	pspell_manager_store_replacement(spell->manager, 
			oldword, strlen(oldword),
			newword, strlen(newword));

	g_free(oldword);
}

GtkWidget*
build_suggestion_menu(GtkSpell *spell, GtkTextBuffer *buffer,
                      const char *word) {
	const char *suggestion;
	GtkWidget *topmenu, *menu;
	GtkWidget *mi;
	GtkWidget *hbox;
	int count = 0;
	void *spelldata;
	const PspellWordList *suggestions;
	PspellStringEmulation *elements;
	char *label;
	
	topmenu = menu = gtk_menu_new();

	/* + Add to Dictionary */
	label = g_strdup_printf("Add \"%s\" to Dictionary", word);
	mi = gtk_image_menu_item_new_with_label(label);
	g_free(label);
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi), 
			gtk_image_new_from_stock(GTK_STOCK_ADD, GTK_ICON_SIZE_MENU));
	g_signal_connect(G_OBJECT(mi), "activate",
			G_CALLBACK(add_to_dictionary), spell);
	gtk_widget_show_all(mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(topmenu), mi);

	/* Separator */
	mi = gtk_menu_item_new();
	gtk_widget_show(mi);
	gtk_menu_shell_append(GTK_MENU_SHELL(topmenu), mi);

	suggestions = pspell_manager_suggest(spell->manager, word, -1);
	elements = pspell_word_list_elements(suggestions);

	suggestion = pspell_string_emulation_next(elements);
	if (suggestion == NULL) {
		/* no suggestions.  put something in the menu anyway... */
		GtkWidget *label;
		label = gtk_label_new("");
		gtk_label_set_markup(GTK_LABEL(label), "<i>(no suggestions)</i>");

		mi = gtk_menu_item_new();
		gtk_container_add(GTK_CONTAINER(mi), label);
		gtk_widget_show_all(mi);
		gtk_menu_shell_prepend(GTK_MENU_SHELL(menu), mi);
	} else {
		/* build a set of menus with suggestions. */
		while (suggestion != NULL) {
			if (count == 10) {
				mi = gtk_menu_item_new();
				gtk_widget_show(mi);
				gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);

				mi = gtk_menu_item_new_with_label("More...");
				gtk_widget_show(mi);
				gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);

				menu = gtk_menu_new();
				gtk_menu_item_set_submenu(GTK_MENU_ITEM(mi), menu);
				count = 0;
			}
			mi = gtk_menu_item_new_with_label(suggestion);
			g_signal_connect(G_OBJECT(mi), "activate",
					G_CALLBACK(replace_word), spell);
			gtk_widget_show(mi);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);
			count++;
			suggestion = pspell_string_emulation_next(elements);
		}
	}

	delete_pspell_string_emulation(elements);

	return topmenu;
}

static void
populate_popup(GtkTextView *textview, GtkMenu *menu, GtkSpell *spell) {
	GtkWidget *img, *mi;
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
	GtkTextIter start, end;
	char *word;

	/* we need to figure out if they picked a misspelled word. */
	get_cur_word_extents(buffer, &start, &end);

	/* if our highlight algorithm ever messes up, 
	 * this isn't correct, either. */
	if (!gtk_text_iter_has_tag(&start, spell->tag_highlight))
		return; /* word wasn't misspelled. */

	/* menu separator comes first. */
	mi = gtk_menu_item_new();
	gtk_widget_show(mi);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(menu), mi);

	/* then, on top of it, the suggestions menu. */
	img = gtk_image_new_from_stock(GTK_STOCK_SPELL_CHECK, GTK_ICON_SIZE_MENU);
	mi = gtk_image_menu_item_new_with_label("Spelling Suggestions");
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(mi), img);

	word = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mi),
			build_suggestion_menu(spell, buffer, word));
	g_free(word);

	gtk_widget_show_all(mi);
	gtk_menu_shell_prepend(GTK_MENU_SHELL(menu), mi);
}

/* when the user right-clicks on a word, they want to check that word.
 * here, we move the cursor to the location of the clicked-upon word.
 * is this necessary?  we could maybe just check the word from its
 * existing location... */
static gboolean
button_press_event(GtkTextView *view, GdkEventButton *event, gpointer data) {
	if (event->button == 3) {
		gint x, y;
		GtkTextIter iter;

		gtk_text_view_window_to_buffer_coords(view, 
				GTK_TEXT_WINDOW_TEXT, 
				event->x, event->y,
				&x, &y);
		gtk_text_view_get_iter_at_location(view, &iter, x, y);
		gtk_text_buffer_place_cursor(gtk_text_view_get_buffer(view),
				&iter);
	}
	return FALSE; /* false: let gtk process this event, too.
					 we don't want to eat any events. */
}

static gboolean
gtkspell_set_language_internal(GtkSpell *spell, const gchar *lang, GError **error) {
	PspellConfig *config;
	PspellCanHaveError *err;
	PspellManager *manager;

	if (lang == NULL) {
		lang = g_getenv("LANG");
		if (lang) {
			if (g_strncasecmp(lang, "C", 1) == 0)
				lang = NULL;
			else if (lang[0] == 0)
				lang = NULL;
		}
	}

	config = new_pspell_config();
	if (lang)
		pspell_config_replace(config, "language-tag", lang);
	pspell_config_replace(config, "encoding", "utf-8");
	err = new_pspell_manager(config);
	delete_pspell_config(config);

	if (pspell_error_number(err) != 0) {
		g_set_error(error, GTKSPELL_ERROR, GTKSPELL_ERROR_PSPELL,
				"pspell: %s", pspell_error_message(err));
		return FALSE;
	} 
	if (spell->manager)
		delete_pspell_manager(spell->manager);
	spell->manager = to_pspell_manager(err);

	return TRUE;
}

gboolean
gtkspell_set_language(GtkSpell *spell, const gchar *lang, GError **error) {
	gboolean ret;

	if (error)
		g_return_val_if_fail(*error == NULL, FALSE);

	ret = gtkspell_set_language_internal(spell, lang, error);
	if (ret)
		gtkspell_recheck_all(spell);

	return ret;
}

void
gtkspell_recheck_all(GtkSpell *spell) {
	GtkTextBuffer *buffer;
	GtkTextIter start, end;

	buffer = gtk_text_view_get_buffer(spell->view);

	gtk_text_buffer_get_bounds(buffer, &start, &end);

	check_range(spell, buffer, start, end);
}

GtkSpell*
gtkspell_new_attach(GtkTextView *view, const gchar *lang, GError **error) {
	GtkTextBuffer *buffer;
	GtkTextIter start, end;

	GtkSpell *spell;

	if (error)
		g_return_val_if_fail(*error == NULL, NULL);

	spell = g_object_get_data(G_OBJECT(view), GTKSPELL_OBJECT_KEY);
	g_assert(spell == NULL);

	/* attach to the widget */
	spell = g_new0(GtkSpell, 1);
	spell->view = view;
	if (!gtkspell_set_language_internal(spell, lang, error)) {
		g_free(spell);
		return NULL;
	}
	g_object_set_data(G_OBJECT(view), GTKSPELL_OBJECT_KEY, spell);

	g_signal_connect_swapped(G_OBJECT(view), "destroy",
			G_CALLBACK(gtkspell_free), spell);
	g_signal_connect(G_OBJECT(view), "button-press-event",
			G_CALLBACK(button_press_event), spell);
	g_signal_connect(G_OBJECT(view), "populate-popup",
			G_CALLBACK(populate_popup), spell);

	buffer = gtk_text_view_get_buffer(view);

	g_signal_connect(G_OBJECT(buffer),
			"insert-text",
			G_CALLBACK(insert_text_before), spell);
	g_signal_connect_after(G_OBJECT(buffer),
			"insert-text",
			G_CALLBACK(insert_text_after), spell);
	g_signal_connect_after(G_OBJECT(buffer),
			"delete-range",
			G_CALLBACK(delete_range_after), spell);

	spell->tag_highlight = gtk_text_buffer_create_tag(buffer,
			"gtkspell-misspelled",
			"foreground", "red", 
			"underline", PANGO_UNDERLINE_SINGLE,
			NULL);

	/* we create the mark here, but we don't use it until text is
	 * inserted, so we don't really care where iter points.  */
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	spell->mark_insert = gtk_text_buffer_create_mark(buffer,
			"gtkspell-insert",
			&start, TRUE);

	/* now check the entire text buffer. */
	gtkspell_recheck_all(spell);
	return spell;
}

static void
gtkspell_free(GtkSpell *spell) {
	GtkTextBuffer *buffer;
	GtkTextTagTable *table;
	GtkTextIter start, end;

	buffer = gtk_text_view_get_buffer(spell->view);
	table = gtk_text_buffer_get_tag_table(buffer);

	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gtk_text_buffer_remove_tag(buffer, spell->tag_highlight, &start, &end);
	gtk_text_tag_table_remove(table, spell->tag_highlight);

	gtk_text_buffer_delete_mark(buffer, spell->mark_insert);

	delete_pspell_manager(spell->manager);

	g_signal_handlers_disconnect_matched(spell->view,
			G_SIGNAL_MATCH_DATA,
			0, 0, NULL, NULL,
			spell);
	g_signal_handlers_disconnect_matched(buffer,
			G_SIGNAL_MATCH_DATA,
			0, 0, NULL, NULL,
			spell);
	g_free(spell);
}

GtkSpell*
gtkspell_get_from_text_view(GtkTextView *view) {
	return g_object_get_data(G_OBJECT(view), GTKSPELL_OBJECT_KEY);
}
	
void
gtkspell_detach(GtkSpell *spell) {
	g_return_if_fail(spell != NULL);

	g_object_set_data(G_OBJECT(spell->view), GTKSPELL_OBJECT_KEY, NULL);
	gtkspell_free(spell);
}

/* backward compatibility functions. */

int
gtkspell_init() {
	/* we do nothing. */
	return 0;
}

void
gtkspell_attach(GtkTextView *view) {
	gtkspell_new_attach(view, NULL, NULL);
}
