/*******************************************************************************
 **3456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
 **      10        20        30        40        50        60        70        80
 **
 ** program: sdl_with_gtk
 **
 ** author: Mirco Mueller (mirco@easyfish.de)
 **
 ** notes: this example demonstrates how to use gtk+ funtionality
 ** (e.g.GtkFileChooser) from within a SDL-based program without the usual
 ** problems of hickups of the event-loops of SDL and gtk+ getting in each others
 ** way
 **
 ** once the program runs you can use the following keys:
 **
 **	q/ESC - exit program
 **	1 - open a file-dialog (using GtkFileChooser)
 **	2 - print the screen-width to stdout (using gdk calls)
 **	3 - print the screen-height to stdout (using gdk calls)
 **
 ** In order to compile this program you will need a recent version of SDL and
 ** gtk+ 2.4.x along with the needed development files on your Linux machine. I
 ** only tested it on Fedora Core2, but it will very likely compile on other
 ** recent Linux distros.
 **
 *******************************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include <SDL/SDL.h>

void dialog_destroy(GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy(widget);
    gtk_main_quit();
}

void dialog_accept(GtkWidget *widget, gpointer data)
{
    GSList *list;
    unsigned int i;

    list = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(widget));
    for (i = 0; i < g_slist_length(list); i++)
    {
        printf("file: %s\n", (char *)g_slist_nth_data(list, i));
    }
    for (i = 0; i < g_slist_length(list); i++)
    {
        g_free(g_slist_nth_data(list, i));
    }
    g_slist_free(list);

    gtk_widget_destroy(widget);
    gtk_main_quit();
}

void dialog_cancel(GtkWidget *widget, gpointer data)
{
    gtk_widget_destroy(widget);
    gtk_main_quit();
}

void dialog_response(GtkDialog *dialog, gint arg, gpointer data)
{
    switch (arg)
    {
    case GTK_RESPONSE_ACCEPT:
        dialog_accept(GTK_WIDGET(dialog), data);
        break;

    case GTK_RESPONSE_CANCEL:
        dialog_cancel(GTK_WIDGET(dialog), data);
        break;
    }
}

int get_screen_width(int argc, char **argv)
{
    GdkDisplay *p_defaultDisplay = NULL;
    GdkScreen *p_defaultScreen = NULL;
    gint i_screenWidth = 0;

    gtk_init(&argc, &argv);
    gdk_threads_enter();

    p_defaultDisplay = gdk_display_get_default();
    if (p_defaultDisplay)
    {
        p_defaultScreen = gdk_display_get_default_screen(p_defaultDisplay);
    }
    if (p_defaultScreen)
    {
        i_screenWidth = gdk_screen_get_width(p_defaultScreen);
    }
    gdk_threads_leave();

    return i_screenWidth;
}

int get_screen_height(int argc, char **argv)
{
    GdkDisplay *p_defaultDisplay = NULL;
    GdkScreen *p_defaultScreen = NULL;
    gint i_screenHeight = 0;

    gtk_init(&argc, &argv);
    gdk_threads_enter();

    p_defaultDisplay = gdk_display_get_default();
    if (p_defaultDisplay)
    {
        p_defaultScreen = gdk_display_get_default_screen(p_defaultDisplay);
    }
    if (p_defaultScreen)
    {
        i_screenHeight = gdk_screen_get_height(p_defaultScreen);
    }
    gdk_threads_leave();

    return i_screenHeight;
}

void open_dialog(int argc, char **argv)
{
    GtkWidget *fileDialog;

    gtk_init(&argc, &argv);
    fileDialog = gtk_file_chooser_dialog_new("select some stuff", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL,
                 GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(fileDialog), 1);
    g_signal_connect(G_OBJECT(fileDialog), "destroy", G_CALLBACK(dialog_destroy), NULL);
    g_signal_connect(G_OBJECT(fileDialog), "response", G_CALLBACK(dialog_response), NULL);
    gtk_widget_show(fileDialog);

    gdk_threads_enter();
    gtk_main();
    gdk_threads_leave();
}

int main(int argc, char **argv)
{
    SDL_Surface *window = NULL;
    SDL_Event event;
    SDL_Rect rect;
    int run = 1;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetCaption("SDL using GtkFileChooser", NULL);

    window = SDL_SetVideoMode(640, 480, 0, SDL_HWSURFACE);

    rect.x = 0;
    rect.y = 0;
    rect.w = window->w;
    rect.h = window->h;

    SDL_FillRect(window, &rect, 0);

    /* this call in only allowed once in a program */
    g_thread_init(NULL);
    while (run)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                run = 0;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_1)
                {
                    open_dialog(argc, argv);
                }
                if (event.key.keysym.sym == SDLK_2)
                {
                    printf("screen width: %d\n", get_screen_width(argc, argv));
                }
                if (event.key.keysym.sym == SDLK_3)
                {
                    printf("screen height: %d\n", get_screen_height(argc, argv));
                }
                if (event.key.keysym.sym == SDLK_q)
                {
                    run = 0;
                }
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    run = 0;
                }
                break;
            }
        }
    }
    return 0;
}
