#ifndef ARBOL_EVALUACION
#define ARBOL_EVALUACION

#include <glib.h>
#include <gtk/gtk.h>

gboolean evaluar_expresion_logica(const gchar *expr, GList *proposiciones);
void exportar_arbol_dot(const gchar *expr, const gchar *ruta_archivo);
void exportar_arbol_dot_etiquetado(const gchar *expr, const gchar *ruta_archivo);


#endif // ARBOL_EVALUACION
