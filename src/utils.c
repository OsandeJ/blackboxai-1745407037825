#include "utils.h"

void formatar_kwanza(double valor, char *buffer, size_t size) {
    snprintf(buffer, size, "%.2f Kz", valor);
}

void obter_data_atual(char *buffer, size_t size) {
    time_t now;
    struct tm *tm_info;
    
    time(&now);
    tm_info = localtime(&now);
    strftime(buffer, size, "%d/%m/%Y", tm_info);
}

void obter_hora_atual(char *buffer, size_t size) {
    time_t now;
    struct tm *tm_info;
    
    time(&now);
    tm_info = localtime(&now);
    strftime(buffer, size, "%H:%M:%S", tm_info);
}

void mostrar_mensagem(GtkWindow *parent, const char *mensagem, GtkMessageType tipo) {
    GtkWidget *dialog = gtk_message_dialog_new(parent,
                                             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                             tipo,
                                             GTK_BUTTONS_OK,
                                             "%s", mensagem);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void mostrar_erro(GtkWindow *parent, const char *mensagem) {
    mostrar_mensagem(parent, mensagem, GTK_MESSAGE_ERROR);
}

void mostrar_sucesso(GtkWindow *parent, const char *mensagem) {
    mostrar_mensagem(parent, mensagem, GTK_MESSAGE_INFO);
}

gboolean validar_numero(const char *texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        if (!g_ascii_isdigit(texto[i]) && texto[i] != '.') {
            return FALSE;
        }
    }
    return TRUE;
}

gboolean validar_telefone(const char *texto) {
    if (strlen(texto) != 9 || texto[0] != '9') {
        return FALSE;
    }
    
    for (int i = 0; texto[i] != '\0'; i++) {
        if (!g_ascii_isdigit(texto[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

gboolean validar_nif(const char *texto) {
    if (strlen(texto) != 9) {
        return FALSE;
    }
    
    for (int i = 0; texto[i] != '\0'; i++) {
        if (!g_ascii_isdigit(texto[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

void inicializar_configuracoes_regionais(void) {
    setlocale(LC_ALL, "pt_AO.UTF-8");
    if (strcmp(setlocale(LC_ALL, NULL), "pt_AO.UTF-8") != 0) {
        setlocale(LC_ALL, "pt_PT.UTF-8");
    }
}
