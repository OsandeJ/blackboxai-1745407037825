#include <gtk/gtk.h>
#include <locale.h>
#include "database.h"
#include "utils.h"
#include "ui/main_window.h"

static void aplicar_estilo_css(void) {
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "/* Estilo para valores monetários */\n"
        ".valor-calculado {\n"
        "    font-size: 14px;\n"
        "    color: #2c3e50;\n"
        "}\n"
        ".valor-total {\n"
        "    font-weight: bold;\n"
        "    color: #27ae60;\n"
        "}\n"
        "/* Estilo para botões de ação */\n"
        "button {\n"
        "    padding: 5px 10px;\n"
        "}\n"
        "button.destructive-action {\n"
        "    background-color: #e74c3c;\n"
        "    color: white;\n"
        "}\n"
        "button.suggested-action {\n"
        "    background-color: #2ecc71;\n"
        "    color: white;\n"
        "}\n"
        "/* Estilo para cabeçalhos */\n"
        "headerbar {\n"
        "    background: linear-gradient(to right, #2c3e50, #3498db);\n"
        "    color: white;\n"
        "}\n"
        "/* Estilo para frames */\n"
        "frame > label {\n"
        "    font-weight: bold;\n"
        "    color: #2c3e50;\n"
        "}\n"
        "/* Estilo para TreeView */\n"
        "treeview {\n"
        "    font-size: 12px;\n"
        "}\n"
        "treeview:selected {\n"
        "    background-color: #3498db;\n"
        "    color: white;\n"
        "}\n"
        "/* Estilo para mensagens de erro */\n"
        ".error-message {\n"
        "    color: #c0392b;\n"
        "    font-weight: bold;\n"
        "}\n"
        "/* Estilo para mensagens de sucesso */\n"
        ".success-message {\n"
        "    color: #27ae60;\n"
        "    font-weight: bold;\n"
        "}\n",
        -1, NULL);
    
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                            GTK_STYLE_PROVIDER(provider),
                                            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(provider);
}

static void on_activate(GtkApplication *app, gpointer user_data) {
    (void)app;        // Parâmetro intencionalmente não utilizado
    (void)user_data;  // Parâmetro intencionalmente não utilizado
    
    // Configurar localização para Angola (ou português genérico se não disponível)
    setlocale(LC_ALL, "pt_AO.UTF-8");
    if (strcmp(setlocale(LC_ALL, NULL), "pt_AO.UTF-8") != 0) {
        setlocale(LC_ALL, "pt_PT.UTF-8");
    }
    
    // Inicializar banco de dados
    if (!inicializar_database()) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                 GTK_DIALOG_MODAL,
                                                 GTK_MESSAGE_ERROR,
                                                 GTK_BUTTONS_OK,
                                                 "Erro ao inicializar o banco de dados!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Aplicar estilo CSS
    aplicar_estilo_css();
    
    // Criar e mostrar a janela principal
    criar_janela_principal();
    
    // Iniciar o loop principal do GTK
    gtk_main();
}

int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("com.example.sistema_gestao",
                            G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    // Finalizar banco de dados
    finalizar_database();
    
    return status;
}
