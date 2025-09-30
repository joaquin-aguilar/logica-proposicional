#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#include <gtk/gtk.h>
#include "../proposiciones/arbol_evaluacion.h"
typedef struct 
{
    GtkWidget *boton;
    char letra;
    gboolean valor;
} Proposicion;

typedef struct 
{
    GtkWidget *caja;
    GtkWidget *operacion_logica;
    GtkWidget *contrasenna;
    int contador;
    GList *proposiciones;
} DatosVentana;

void al_activar(GtkApplication* programa, gpointer informacion_recuperada);
GtkWidget *crear_proposicion(DatosVentana *datos_ventana, gboolean valor, GtkWidget *boton_origen);


#endif
