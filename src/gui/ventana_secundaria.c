#include "ventana_secundaria.h"
#include <gtk/gtk.h>
#include <glib.h>
#include "../proposiciones/filtros.h"
#include "ventana_principal.h" 

typedef struct 
{
    GtkEntry *entrada;
    GtkDropDown *menu_desplegable;
    gboolean *resultado;
    GtkWindow *ventana;
    const char *contrasenna;
    GtkWidget *boton_destino;
} DatosFormulario;

static int string_a_entero(const char *str) 
{
    GError *error = NULL;
    gint64 valor;
    gboolean ok = g_ascii_string_to_signed(str, 10, G_MININT64, G_MAXINT64, &valor, &error);
    if (ok) 
        return (int)valor;
    else 
    {
        g_clear_error(&error);
        return -1;
    }
}

static void al_pulsar_ok(GtkButton *boton, gpointer user_data) 
{
    DatosFormulario *datos_formulario = (DatosFormulario *)user_data;

    if (!datos_formulario || !datos_formulario->entrada || !datos_formulario->menu_desplegable ||
        !datos_formulario->resultado || !datos_formulario->ventana || !datos_formulario->contrasenna ||
        !datos_formulario->boton_destino) {
        g_warning("Datos incompletos en callback");
        return;
    }

    const gchar *text = gtk_editable_get_text(GTK_EDITABLE(datos_formulario->entrada));
    guint index = gtk_drop_down_get_selected(datos_formulario->menu_desplegable);
    int input_numero = string_a_entero(text);

    *(datos_formulario->resultado) = FALSE;

    if (text && *text != '\0' && index != GTK_INVALID_LIST_POSITION) 
    {
        if (input_numero >= 0) 
        {
            int longitud = datos_formulario->contrasenna ? (int)strlen(datos_formulario->contrasenna) : 0;
            switch (index) 
            {
                case MAYOR_A:
                    *(datos_formulario->resultado) = tamanyo_mayor_a(longitud, input_numero);
                    break;
                case MENOR_A:
                    *(datos_formulario->resultado) = tamanyo_menor_a(longitud, input_numero);
                    break;
                case IGUAL_A:
                    *(datos_formulario->resultado) = tamanyo_igual_a(longitud, input_numero);
                    break;
                case CONTIENE_A:
                    if (text && *text != '\0') 
                    {
                        *(datos_formulario->resultado) = contiene_caracter(datos_formulario->contrasenna, text[0]);
                        g_print("%s contiene al caracter: %s?\n", datos_formulario->contrasenna,contiene_caracter(datos_formulario->contrasenna, text[0]) ? "Si" : "No");
                    }
                    else
                        *(datos_formulario->resultado) = FALSE;
                    break;
                default:
                    *(datos_formulario->resultado) = FALSE;
                    break;
            }
        }
    }

    DatosVentana *datos_ventana = g_object_get_data(G_OBJECT(datos_formulario->boton_destino), "datos-ventana");
    if (datos_ventana) 
    {
        GtkWidget *boton_nuevo = crear_proposicion(datos_ventana, *(datos_formulario->resultado), NULL);
        gchar *tooltip = NULL;
        const gchar *apoyo = *(datos_formulario->resultado) ? "Verdadero" : "Falso";

        switch (index) 
        {
            case MAYOR_A:
                tooltip = g_strdup_printf("Tiene mas de '%i' caracteres? (%s)", input_numero, apoyo);
                break;
            case MENOR_A:
                tooltip = g_strdup_printf("Tiene menos de '%i' caracteres? (%s)", input_numero, apoyo);
                break;
            case IGUAL_A:
                tooltip = g_strdup_printf("Tiene exactamente '%i' caracteres? (%s)", input_numero, apoyo);
                break;
            case CONTIENE_A:
                if (text && *text != '\0') 
                    tooltip = g_strdup_printf("Contiene al caracter '%c'? (%s)", text[0], apoyo);
                else 
                    tooltip = g_strdup("Error al evaluar la proposicion! (F)");
                break;
            default:
                tooltip = g_strdup("Error al evaluar la proposicion! (F)");
                break;
        }

        gtk_widget_set_tooltip_text(boton_nuevo, tooltip);
        g_free(tooltip);
    }

    gtk_window_destroy(datos_formulario->ventana);
    g_free(datos_formulario);
}


void mostrar_ventana_proposicional(GtkWindow *parent, const gchar *mensaje, gboolean *resultado, GtkWidget *boton_destino, const char* contrasenna) 
{
    GtkWidget *ventana = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(ventana), mensaje);
    gtk_window_set_transient_for(GTK_WINDOW(ventana), parent);
    gtk_window_set_modal(GTK_WINDOW(ventana), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(ventana), 300, 150);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(ventana), box);
    gtk_widget_set_margin_top(box, 20);
    gtk_widget_set_margin_bottom(box, 20);
    gtk_widget_set_margin_start(box, 20);
    gtk_widget_set_margin_end(box, 20);

    GtkEntry *entry = GTK_ENTRY(gtk_entry_new());
    gtk_widget_set_margin_bottom(GTK_WIDGET(entry), 10);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(entry));
    gtk_widget_grab_focus(GTK_WIDGET(entry));

    const gchar *opciones[] = 
    {
        arr_filtros[0], arr_filtros[1], arr_filtros[2], arr_filtros[3], NULL
    };
    GtkStringList *list = gtk_string_list_new(opciones);
    GtkDropDown *menu_desplegable = GTK_DROP_DOWN(gtk_drop_down_new(G_LIST_MODEL(list), NULL));
    gtk_widget_set_margin_bottom(GTK_WIDGET(menu_desplegable), 10);
    gtk_box_append(GTK_BOX(box), GTK_WIDGET(menu_desplegable));

    GtkWidget *boton = gtk_button_new_with_label("OK");
    gtk_box_append(GTK_BOX(box), boton);

    DatosFormulario *data = g_malloc(sizeof(DatosFormulario));
    data->entrada = entry;
    data->menu_desplegable = menu_desplegable;
    data->resultado = resultado;
    data->ventana = GTK_WINDOW(ventana);
    data->contrasenna = contrasenna;
    data->boton_destino = boton_destino;

    g_print("Evaluando con contrasenna: '%s', tamanno: %i\n", data->contrasenna, (int) strlen(data->contrasenna));
    
    g_signal_connect(boton, "clicked", G_CALLBACK(al_pulsar_ok), data);

    gtk_window_present(GTK_WINDOW(ventana));
}
