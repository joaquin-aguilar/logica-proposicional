#include <gtk/gtk.h>

#include "gui/interfaz_grafica.h"


int main(int argc, char** argv)
{
    GtkApplication *aplicacion = gtk_application_new("com.joaquin-aguilar.logica-proposicional", G_APPLICATION_DEFAULT_FLAGS);

    g_signal_connect(aplicacion, "activate", G_CALLBACK(al_activar), NULL);

    int estado = g_application_run(G_APPLICATION(aplicacion), argc, argv);

    g_object_unref(aplicacion);

    return estado;
} 