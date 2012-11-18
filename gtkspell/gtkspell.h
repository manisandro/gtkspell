/* gtkspell - a spell-checking addon for GTK's TextView widget
 * Copyright (c) 2002 Evan Martin
 * Copyright (c) 2012 Sandro Mani
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/* vim: set ts=4 sw=4 wm=5 : */

#ifndef GTK_SPELL_H
#define GTK_SPELL_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

/**
* GTK_SPELL_ERROR:
*
* Error domain used for gtkspell operations. Indicates that the error code
* will be in the #GtkSpellError enumeration.
*/
#define GTK_SPELL_ERROR gtk_spell_error_quark()

/**
* GtkSpellError:
* @GTK_SPELL_ERROR_BACKEND: Error code for backend errors
*
* Error codes used for GtkSpell errors.
*
**/
typedef enum {
  GTK_SPELL_ERROR_BACKEND
} GtkSpellError;

GQuark gtk_spell_error_quark(void);

#define GTK_SPELL_TYPE_CHECKER            (gtk_spell_checker_get_type ())
#define GTK_SPELL_CHECKER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SPELL_TYPE_CHECKER, GtkSpellChecker))
#define GTK_SPELL_IS_CHECKER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SPELL_TYPE_CHECKER))
#define GTK_SPELL_CHECKER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SPELL_TYPE_CHECKER, GtkSpellCheckerClass))
#define GTK_SPELL_IS_CHECKER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SPELL_TYPE_CHECKER))
#define GTK_SPELL_CHECKER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SPELL_TYPE_CHECKER, GtkSpellCheckerClass))

/**
 * GtkSpellChecker:
 *
 * The #GtkSpellChecker struct contains only private fields.
 */
typedef struct _GtkSpellChecker        GtkSpellChecker;
typedef struct _GtkSpellCheckerClass   GtkSpellCheckerClass;

GType            gtk_spell_checker_get_type             (void) G_GNUC_CONST;
GtkSpellChecker *gtk_spell_checker_new                  ();
gboolean         gtk_spell_checker_attach               (GtkSpellChecker *spell,
                                                         GtkTextView   *view);
GtkSpellChecker *gtk_spell_checker_get_from_text_view   (GtkTextView   *view);
void             gtk_spell_checker_detach               (GtkSpellChecker *spell);
GtkWidget       *gtk_spell_checker_get_suggestions_menu (GtkSpellChecker *spell,
                                                         GtkTextIter   *iter);
gboolean         gtk_spell_checker_set_language         (GtkSpellChecker *spell,
                                                         const gchar   *lang,
                                                         GError       **error);
const gchar     *gtk_spell_checker_get_language         (GtkSpellChecker *spell);
void             gtk_spell_checker_recheck_all          (GtkSpellChecker *spell);

G_END_DECLS

#endif /* GTK_SPELL_H */
