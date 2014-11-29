#include <stdlib.h>

#include <webkit/webkit.h>
#include <gtk/gtk.h>

void
web_view_load_status_cb(WebKitWebView *web_view, GParamSpec *param_spec)
{
	static const char *loadStatusString[] = {
		"WEBKIT_LOAD_PROVISIONAL",
		"WEBKIT_LOAD_COMMITTED",
		"WEBKIT_LOAD_FINISHED",
		"WEBKIT_LOAD_FIRST_VISUALLY_NON_EMPTY_LAYOUT",
		"WEBKIT_LOAD_FAILED"
	};

	WebKitLoadStatus loadStatus = webkit_web_view_get_load_status(web_view);
	const char *uri = webkit_web_view_get_uri(web_view);

	printf("notify::load-status: %s: %s\n", loadStatusString[loadStatus], uri);
}

void
web_view_resource_request_starting_cb(WebKitWebView *web_view, WebKitWebFrame *web_frame,
		WebKitWebResource *web_resource, WebKitNetworkRequest *network_request, 
		WebKitNetworkResponse *network_response, gpointer data)
{
	SoupMessage *msg = webkit_network_request_get_message(network_request);
	if (! msg) {
		printf("resource-request-starting: no SoupMessage\n");
		return;
	}

	SoupURI *uri = soup_message_get_uri(msg);
	if (! uri) {
		printf("resource-request-starting: no SoupURI\n");
		return;
	}

	const char *uri_str = webkit_network_request_get_uri(network_request);
	printf("resource-request-starting: uri=%s\n", uri_str);

	/* switch from HTTP to HTTPS */
	if (uri->scheme == SOUP_URI_SCHEME_HTTP && strcmp(uri->host, "projects.dm.id.lv") == 0) {
		soup_uri_set_scheme(uri, SOUP_URI_SCHEME_HTTPS);
		printf("resource-request-starting: switch projects.dm.id.lv from HTTP to HTTPS\n");
	}
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		printf("usage: %s url\n", argv[0]);
		printf(" test without redirect: http://projects.dm.id.lv/s/hsts-testresult.html\n");
		printf(" test with    redirect: http://bit.ly/1sXuZsu\n");
		return(EXIT_FAILURE);
	}

	/* Initialisation */
	gtk_init(&argc, &argv);

	/* Create the widgets */
	GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
	GtkWidget *web_view = webkit_web_view_new();

	/* Place the WebKitWebView in the GtkScrolledWindow */
	gtk_container_add(GTK_CONTAINER(scrolled_window), web_view);
	gtk_container_add(GTK_CONTAINER(main_window), scrolled_window);

	/* Connect some signals */
	g_object_connect(
		G_OBJECT(web_view),
		"signal::notify::load-status", G_CALLBACK(web_view_load_status_cb), NULL,
		"signal::resource-request-starting", G_CALLBACK(web_view_resource_request_starting_cb), NULL,
		NULL);

	/* Open a webpage */
	//webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "http://projects.dm.id.lv/s/hsts-testresult.html");
	//webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), "http://bit.ly/1sXuZsu");
	webkit_web_view_load_uri(WEBKIT_WEB_VIEW(web_view), argv[1]);

	/* Show the result */
	gtk_window_set_default_size(GTK_WINDOW(main_window), 800, 600);
	gtk_widget_show_all(main_window);

	gtk_main();
	return(EXIT_SUCCESS);
}
