#include "ventana_principal.h"
#include "ventana_secundaria.h"

static void annadir_texto(GtkWidget *boton, gpointer informacion) 
{
    DatosVentana *data = (DatosVentana *)informacion;
    const char *texto = gtk_button_get_label(GTK_BUTTON(boton));
    const char *texto_viejo = gtk_editable_get_text(GTK_EDITABLE(data->operacion_logica));
    char *nuevo_texto = g_strconcat(texto_viejo, texto, NULL);
    gtk_editable_set_text(GTK_EDITABLE(data->operacion_logica), nuevo_texto);
    g_free(nuevo_texto);
}

static void al_click_eliminar(GtkWidget *boton, gpointer informacion) 
{
    GtkWidget *boton_a_eliminar = GTK_WIDGET(informacion);
    gtk_widget_unparent(boton_a_eliminar);
}

GtkWidget *crear_proposicion(DatosVentana *datos_ventana, gboolean valor, GtkWidget *boton_origen) 
{
    char letra = 'P' + datos_ventana->contador;
    datos_ventana->contador++;

    GtkWidget *contenedor = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *boton_prop = gtk_button_new_with_label((gchar[]){letra, '\0'});
    g_signal_connect(boton_prop, "clicked", G_CALLBACK(annadir_texto), datos_ventana);
    gtk_box_append(GTK_BOX(contenedor), boton_prop);

    GtkWidget *boton_eliminar = gtk_button_new_with_label("X");
    g_signal_connect(boton_eliminar, "clicked", G_CALLBACK(al_click_eliminar), contenedor);
    gtk_box_append(GTK_BOX(contenedor), boton_eliminar);
    gtk_widget_set_tooltip_text(boton_eliminar, "Eliminar proposición");

    gtk_box_append(GTK_BOX(datos_ventana->caja), contenedor);

    Proposicion *p = g_malloc(sizeof(Proposicion));
    p->boton = boton_prop;
    p->letra = letra;
    p->valor = valor;
    datos_ventana->proposiciones = g_list_append(datos_ventana->proposiciones, p);

    return boton_prop;
}

static void annadir_proposicion(GtkWidget *boton, gpointer informacion) 
{
    DatosVentana *datos_ventana = (DatosVentana*) informacion;

    gboolean *resultado = g_malloc(sizeof(gboolean));
    *resultado = FALSE;

    g_object_set_data(G_OBJECT(boton), "datos-ventana", datos_ventana);
    mostrar_ventana_proposicional(GTK_WINDOW(gtk_widget_get_root(boton)), "Validar proposición", resultado, boton, gtk_editable_get_text(GTK_EDITABLE(datos_ventana->contrasenna)));
}

static void boton_eliminar(GtkWidget *boton, gpointer informacion)
{
    DatosVentana *datos_ventana = (DatosVentana *)informacion;
    const char *texto_entrada_antiguo = gtk_editable_get_text(GTK_EDITABLE(datos_ventana->operacion_logica));
    glong tamanno_str = g_utf8_strlen(texto_entrada_antiguo, -1);
    if (tamanno_str > 0) 
    {
        const char *ultimo_caracter = g_utf8_offset_to_pointer(texto_entrada_antiguo, tamanno_str - 1);
        gsize bytes = ultimo_caracter - texto_entrada_antiguo;
        char *nuevo_string = g_strndup(texto_entrada_antiguo, bytes);
        gtk_editable_set_text(GTK_EDITABLE(datos_ventana->operacion_logica), nuevo_string);
        g_free(nuevo_string);
    }
}

static void boton_evaluar(GtkWidget *boton, gpointer informacion)
{
    DatosVentana *data_ventana = (DatosVentana *)informacion;
    
    for (GList *l = data_ventana->proposiciones; l != NULL; l = l->next) 
    {
        Proposicion *actual = l->data;
        g_print("%c: %s\n", actual->letra, (actual->valor) ? "Verdadero" : "Falso");
    }

    const gchar *expresion = gtk_editable_get_text(GTK_EDITABLE(data_ventana->operacion_logica));
    gchar *mensaje = g_strdup_printf("%s = %s\n", expresion, evaluar_expresion_logica(expresion, data_ventana->proposiciones) ? "Verdadero" : "Falso");

    GtkWidget *ventana_dialogo = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(ventana_dialogo), "Resultado de Evaluación");
    gtk_window_set_transient_for(GTK_WINDOW(ventana_dialogo), GTK_WINDOW(gtk_widget_get_root(boton)));
    gtk_window_set_modal(GTK_WINDOW(ventana_dialogo), TRUE);
    gtk_window_set_default_size(GTK_WINDOW(ventana_dialogo), 320, 200);

    GtkWidget *contenedor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(contenedor, 20);
    gtk_widget_set_margin_bottom(contenedor, 20);
    gtk_widget_set_margin_start(contenedor, 20);
    gtk_widget_set_margin_end(contenedor, 20);
    gtk_window_set_child(GTK_WINDOW(ventana_dialogo), contenedor);

    
    GtkWidget *label = gtk_label_new(mensaje);
    gtk_box_append(GTK_BOX(contenedor), label);
    
    GtkWidget *boton_ok = gtk_button_new_with_label("OK");
    gtk_box_append(GTK_BOX(contenedor), boton_ok);
    g_signal_connect_swapped(boton_ok, "clicked", G_CALLBACK(gtk_window_destroy), ventana_dialogo);

    exportar_arbol_dot_etiquetado(expresion, "assets/grafo.dot");
    system("dot -Tpng assets/grafo.dot -o assets/resultado.png");
    GFile *file = g_file_new_for_path("assets/resultado.png");
    GtkWidget *imagen = gtk_picture_new_for_file(file);
    g_object_unref(file);
    gtk_widget_set_hexpand(imagen, TRUE);
    gtk_widget_set_vexpand(imagen, TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(imagen), TRUE);
    gtk_box_append(GTK_BOX(contenedor), imagen);
    
    gtk_window_present(GTK_WINDOW(ventana_dialogo));
    g_free(mensaje);
}





void al_activar(GtkApplication* programa, gpointer informacion_recuperada) 
{
    DatosVentana *informacion_ventana = g_new0(DatosVentana, 1);

    GtkWidget *ventana = gtk_application_window_new(GTK_APPLICATION(programa));
    gtk_window_set_title(GTK_WINDOW(ventana), "Calculadora Lógica");
    gtk_window_set_default_size(GTK_WINDOW(ventana), 600, 420);

    GtkWidget *contenedor_principal = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_window_set_child(GTK_WINDOW(ventana), contenedor_principal);

    GtkWidget *comentario_contrasenna = gtk_label_new("Ingresa la contraseña por favor:");
    gtk_box_append(GTK_BOX(contenedor_principal), comentario_contrasenna);

    informacion_ventana->contrasenna = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(informacion_ventana->contrasenna), TRUE);
    gtk_box_append(GTK_BOX(contenedor_principal), informacion_ventana->contrasenna);

    GtkWidget *comentario_expresion = gtk_label_new("Operacion Logica:");
    gtk_box_append(GTK_BOX(contenedor_principal), comentario_expresion);

    informacion_ventana->operacion_logica = gtk_entry_new();
    gtk_editable_set_editable(GTK_EDITABLE(informacion_ventana->operacion_logica), FALSE);
    gtk_box_append(GTK_BOX(contenedor_principal), informacion_ventana->operacion_logica);

    GtkWidget *contenedor_bifurcado = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(contenedor_principal), contenedor_bifurcado);

    informacion_ventana->caja = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_box_append(GTK_BOX(contenedor_bifurcado), informacion_ventana->caja);

    GtkWidget *boton_annadir = gtk_button_new_with_label("+");
    g_object_set_data(G_OBJECT(boton_annadir), "datos-ventana", informacion_ventana);
    g_signal_connect(boton_annadir, "clicked", G_CALLBACK(annadir_proposicion), informacion_ventana);
    gtk_box_append(GTK_BOX(informacion_ventana->caja), boton_annadir);
    gtk_widget_set_tooltip_text(boton_annadir, "Añadir proposición");

    GtkWidget *grilla_operadores = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grilla_operadores), 6);
    gtk_grid_set_column_spacing(GTK_GRID(grilla_operadores), 6);
    gtk_box_append(GTK_BOX(contenedor_bifurcado), grilla_operadores);

    const char *operadores[] = {"->", "<->", "^", "v", "(", ")", "~", "del", "Evaluar"};
    int it_1 = 0;
    for (int it_2 = 0; it_2 < 3; it_2++) {
        for (int it_3 = 0; it_3 < 3; it_3++) {
            const char *lbl = operadores[it_1];
            GtkWidget *boton_operador = gtk_button_new_with_label(lbl);
            gtk_grid_attach(GTK_GRID(grilla_operadores), boton_operador, it_3, it_2, 1, 1);

            if (g_strcmp0(lbl, "del") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"Eliminar ultimo");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(boton_eliminar), informacion_ventana);
            }
            else if (g_strcmp0(lbl, "Evaluar") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"Evluar expresión");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(boton_evaluar), informacion_ventana);
            }
            else if (g_strcmp0(lbl, "->") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"Entonces (condicional)");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(annadir_texto), informacion_ventana);
            }
            else if (g_strcmp0(lbl, "<->") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"Si y solo si (bicondicional)");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(annadir_texto), informacion_ventana);
            }

            else if (g_strcmp0(lbl, "^") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"Y (conjunción)");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(annadir_texto), informacion_ventana);
            }
            else if (g_strcmp0(lbl, "v") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"O (disyunción)");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(annadir_texto), informacion_ventana);
            }
            else if (g_strcmp0(lbl, "~") == 0)
            {
                gtk_widget_set_tooltip_text(boton_operador,"No (Negación)");
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(annadir_texto), informacion_ventana);
            }
            else 
                g_signal_connect(boton_operador, "clicked", G_CALLBACK(annadir_texto), informacion_ventana);
            it_1++;
        }
    }

    g_object_set_data_full(G_OBJECT(ventana), "informacion-ventana", informacion_ventana, g_free);
    gtk_window_present(GTK_WINDOW(ventana));
}
