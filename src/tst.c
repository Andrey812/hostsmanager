//gcc base.c -o base `pkg-config --cflags --libs gtk+-2.0`

#include <gtk/gtk.h>

void closeApp(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}


int main (int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *label1, *label2, *label3;
    GtkWidget *vbox;

    /* Initialize the GTK+ and all of its supporting libraries. */
    gtk_init (&argc, &argv);

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(closeApp), NULL);

    // Main window settings
    gtk_window_set_title (GTK_WINDOW (window), "My App");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size (GTK_WINDOW(window), 500, 50);

    // Set labels
    label1 = gtk_label_new("Label 1");
    label2 = gtk_label_new("Label 2");
    label3 = gtk_label_new("Label 3");

    // Set box
    vbox = gtk_vbox_new(FALSE, 10);

    // Pack elements to the box
    gtk_box_pack_start(GTK_BOX(vbox), label1, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), label2, TRUE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_widget_show_all(window);

    /* Hand control over to the main loop. */
    gtk_main ();
    return 0;
}


