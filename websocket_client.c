#include <glib.h>
#include <glib-unix.h>
#include <libsoup/soup.h>

static gchar   *ws_server_addr = "echo.websocket.org";
static gint     ws_server_port = 80;
static gboolean is_wss = FALSE;
static GMainLoop *main_loop;

static GOptionEntry opt_entries[] = {
    {"address", 'a', 0, G_OPTION_ARG_STRING, &ws_server_addr, "Websocket server address (default: echo.websocket.org)", NULL},
	{"port",    'p', 0, G_OPTION_ARG_INT,    &ws_server_port, "Websocket server port (default: 80)", NULL},
	{"is_wss",  's', 0, G_OPTION_ARG_NONE,   &is_wss,         "Enable wss connection (default: disabled)", NULL},
    {NULL},
};

static gboolean sig_handler(gpointer data)
{
    g_main_loop_quit(main_loop);
    return G_SOURCE_REMOVE;
}

static void on_message(SoupWebsocketConnection *conn, gint type, GBytes *message, gpointer data)
{
    if (type == SOUP_WEBSOCKET_DATA_TEXT) {
        gsize sz;
        const gchar *ptr;

        ptr = g_bytes_get_data(message, &sz);
        g_print("Received text data: %s\n", ptr);

        soup_websocket_connection_send_text(conn, (is_wss) ? "Hello Secure Websocket !" : "Hello Websocket !");
    }
    else if (type == SOUP_WEBSOCKET_DATA_BINARY) {
        g_print("Received binary data (not shown)\n");
    }
    else {
        g_print("Invalid data type: %d\n", type);
    }
}

static void on_close(SoupWebsocketConnection *conn, gpointer data)
{
    soup_websocket_connection_close(conn, SOUP_WEBSOCKET_CLOSE_NORMAL, NULL);
    g_print("WebSocket connection closed\n");
}

static void on_connection(SoupSession *session, GAsyncResult *res, gpointer data)
{
    SoupWebsocketConnection *conn;
    GError *error = NULL;

    conn = soup_session_websocket_connect_finish(session, res, &error);
    if (error) {
        g_print("Error: %s\n", error->message);
        g_error_free(error);
        g_main_loop_quit(main_loop);
        return;
    }

    g_signal_connect(conn, "message", G_CALLBACK(on_message), NULL);
    g_signal_connect(conn, "closed",  G_CALLBACK(on_close),   NULL);

    soup_websocket_connection_send_text(conn, (is_wss) ? "Hello Secure Websocket !" : "Hello Websocket !");
}

int main(int argc, char **argv)
{
    GError *error = NULL;
    GOptionContext *context;

    context = g_option_context_new("- WebSocket testing client");
    g_option_context_add_main_entries(context, opt_entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_error_free(error);
        return 1;
    }

    main_loop = g_main_loop_new(NULL, FALSE);

    g_unix_signal_add(SIGINT, (GSourceFunc)sig_handler, NULL);

    SoupSession *session;
    SoupMessage *msg;

    // Create the soup session with WS or WSS
    gchar *uri = NULL;
    session = soup_session_new();
    if (is_wss) {
        // Trick to enable the wss support
        gchar *wss_aliases[] = { "wss", NULL };
        g_object_set(session, SOUP_SESSION_HTTPS_ALIASES, wss_aliases, NULL);
        uri = g_strdup_printf("%s://%s:%d", "wss", ws_server_addr, ws_server_port);
    } else {
        uri = g_strdup_printf("%s://%s:%d", "ws", ws_server_addr, ws_server_port);
    }
    msg = soup_message_new(SOUP_METHOD_GET, uri);
    g_free(uri);

    soup_session_websocket_connect_async(
            session,
            msg,
            NULL, NULL, NULL,
            (GAsyncReadyCallback)on_connection,
            NULL
    );

    g_print("start main loop\n");
    g_main_loop_run(main_loop);

    g_main_loop_unref(main_loop);
    return 0;
}