/* gtkspell - a spell-checking addon for GTK's TextView widget
 * Copyright (c) 2002 Evan Martin.
 */

/* vim: set ts=4 sw=4 wm=5 : */

#ifndef GTKSPELL_H
#define GTKSPELL_H

#define GTKSPELL_ERROR gtkspell_error_quark()

typedef enum {
	GTKSPELL_ERROR_PSPELL,
} GtkSpellError;

GQuark gtkspell_error_quark();

typedef struct _GtkSpell GtkSpell;

/* the idea is to have a GtkSpell object that is analagous to the
 * GtkTextBuffer-- it lives as an attribute of the GtkTextView but
 * it can be referred to directly. */

extern GtkSpell* gtkspell_new_attach(GtkTextView *view,
                                     const gchar *lang, GError **error);
extern GtkSpell* gtkspell_get_from_text_view(GtkTextView *view);
extern void      gtkspell_detach(GtkSpell *spell);

extern gboolean  gtkspell_set_language(GtkSpell *spell,
                                       const gchar *lang, GError **error);

extern void      gtkspell_recheck_all(GtkSpell *spell);


/*** old API-- deprecated. **x*/

extern int gtkspell_init();
/* no-op. */

extern void gtkspell_attach(GtkTextView *view);
/* gtkspell_new_attach(view, NULL, NULL); */

#endif /* GTKSPELL_H */
