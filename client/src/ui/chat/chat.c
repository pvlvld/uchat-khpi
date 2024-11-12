#include "../../../inc/header.h"

static int counter = 0;

typedef struct {
    GtkWidget *widget;
    int *width;
    int *height;
} t_widget_size_data;


gboolean get_widget_size(gpointer data) {
    t_widget_size_data *size_data = (t_widget_size_data *)data;
    GtkWidget *widget = size_data->widget;

    if (gtk_widget_get_mapped(widget)) {
        *size_data->width = gtk_widget_get_allocated_width(widget);
        *size_data->height = gtk_widget_get_allocated_height(widget);

        g_print("height: %d\n", *size_data->height);

        g_free(size_data);

        return FALSE;
    }
    return TRUE;
}

static int draw_chat(GtkWidget *message_wrapper, const char *message_txt, int is_received) {
    gtk_widget_set_halign(message_wrapper, is_received == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
    vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
    int width = 500 - 16 - 8;

	GtkWidget *message = gtk_overlay_new();
    gtk_widget_set_hexpand(message, FALSE);
    gtk_widget_set_halign(message, GTK_ALIGN_FILL);

    vendor.helpers.set_classname_and_id(message, "chat__message");
    gtk_box_pack_start(GTK_BOX(message_wrapper), message, FALSE, FALSE, 0);

//    GtkWidget *message_text_bg = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//    vendor.helpers.set_classname_and_id(message_text_bg, "chat__message__text_bg");
//    gtk_overlay_add_overlay(GTK_OVERLAY(message), message_text_bg);

    GtkWidget *message_text = gtk_label_new(message_txt);
    PangoLayout *layout = gtk_label_get_layout(GTK_LABEL(message_text));
    pango_layout_set_width(layout, width * PANGO_SCALE);
    int height = 18 + pango_layout_get_line_count(layout) * 28;
    gtk_widget_set_size_request(message, width, height);
    gtk_widget_set_halign(message_text, GTK_ALIGN_START);
    gtk_widget_set_size_request(message_text, width, height);
//    gtk_widget_set_size_request(message_text_bg, 500, height);
    gtk_style_context_add_class(gtk_widget_get_style_context(message_text), is_received ? "_received" : "_sended");
    gtk_label_set_line_wrap(GTK_LABEL(message_text), TRUE);
    vendor.helpers.set_classname_and_id(message_text, "chat__message__text");

    gtk_overlay_add_overlay(GTK_OVERLAY(message), message_text);

    GtkWidget *time = gtk_label_new("00:00");

    // Добавляем время в overlay
    gtk_overlay_add_overlay(GTK_OVERLAY(message), time);
    vendor.helpers.set_classname_and_id(time, "chat__message__time");

    gtk_widget_set_margin_start(time, 420);
    gtk_widget_set_margin_bottom(time, 0);
    gtk_widget_set_halign(time, GTK_ALIGN_END);
    gtk_widget_set_valign(time, GTK_ALIGN_END);

    gtk_widget_show_all(message_wrapper);
    return height + 12;
}

void add_chat_message(const char *message_txt) {
    int is_received = rand() % 2;
    GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vendor.pages.main_page.chat.stretchable_box_new_messages), message_wrapper, FALSE, FALSE, 0);

    draw_chat(message_wrapper, message_txt, is_received);
}

static int add_old_chat_message(const char *message_txt) {
    int is_received = rand() % 2;
    GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(message_wrapper, is_received == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
    vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
    gtk_box_pack_end(GTK_BOX(vendor.pages.main_page.chat.stretchable_box_old_messages), message_wrapper, FALSE, FALSE, 0);

	return draw_chat(message_wrapper, message_txt, is_received);
}

gboolean scroll_to_height(gpointer data) {
    t_scroll_data *scroll_data = (t_scroll_data *)data;
    double new_value = gtk_adjustment_get_value(scroll_data->adjustment) + scroll_data->height;
    gtk_adjustment_set_value(scroll_data->adjustment, new_value);

    g_free(scroll_data);
    return FALSE;
}

void on_scroll_value_changed(GtkAdjustment *adjustment, gpointer user_data) {
    (void)user_data;
    GtkWidget *scrolled_window = (GtkWidget *)user_data;

    if (!GTK_IS_SCROLLED_WINDOW(scrolled_window)) {
        g_warning("Provided widget is not a GtkScrolledWindow.");
        return;
    }

    double current_value = gtk_adjustment_get_value(adjustment);

    if (current_value <= gtk_adjustment_get_lower(adjustment) + 200) {
        gint window_width, window_height;
        gtk_window_get_size(GTK_WINDOW(vendor.window), &window_width, &window_height);

        int height = 0;
        (void)height;
        for (unsigned int i = 0; i < 20; i++) {
            char msg[512];
            snprintf(msg, sizeof(msg), "%d: %s", counter, "Scrolled to the top! Scrolled to the top! Scrolled to the top! Scrolled to the top!");
            height += add_old_chat_message(msg);
            counter++;
        }

        GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
        t_scroll_data *scroll_data = g_malloc(sizeof(t_scroll_data));
        scroll_data->adjustment = v_adjustment;
        scroll_data->height = height;

        g_idle_add(scroll_to_height, scroll_data);
    }
}

gboolean set_scroll_to_bottom(gpointer data) {
    GtkAdjustment *vadjustment = GTK_ADJUSTMENT(data);
    gtk_adjustment_set_value(vadjustment, gtk_adjustment_get_upper(vadjustment));
    return FALSE;
}

GtkWidget *chat_init(void) {
    GtkWidget *chat_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.main_page.chat.chat_box = chat_box;
    vendor.helpers.set_classname_and_id(chat_box, "chat");
    gtk_style_context_add_class(gtk_widget_get_style_context(chat_box), "_right_block");
    gtk_widget_set_halign(chat_box, GTK_ALIGN_FILL);
    gtk_widget_set_valign(chat_box, GTK_ALIGN_FILL);
    gtk_widget_set_hexpand(chat_box, TRUE);
    gtk_widget_set_vexpand(chat_box, TRUE);

    GtkWidget *chat_header = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_header, "chat__header");
    gtk_widget_set_size_request(chat_header, -1, 65);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_header, FALSE, FALSE, 0);

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    vendor.pages.main_page.chat.scrolled_window = scrolled_window;
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.helpers.set_classname_and_id(scrolled_window, "chat__scrolled-window");
    gtk_box_pack_start(GTK_BOX(chat_box), scrolled_window, TRUE, TRUE, 0);

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(scrolled_window), content_box);

    GtkWidget *stretchable_box_old_messages = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.main_page.chat.stretchable_box_old_messages = stretchable_box_old_messages;
    gtk_widget_set_vexpand(stretchable_box_old_messages, TRUE);
    gtk_widget_set_valign(stretchable_box_old_messages, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(content_box), stretchable_box_old_messages, TRUE, TRUE, 0);

    GtkWidget *stretchable_box_new_messages = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.main_page.chat.stretchable_box_new_messages = stretchable_box_new_messages;
    gtk_widget_set_vexpand(stretchable_box_new_messages, TRUE);
    gtk_widget_set_valign(stretchable_box_new_messages, GTK_ALIGN_END);
    gtk_box_pack_start(GTK_BOX(content_box), stretchable_box_new_messages, TRUE, TRUE, 0);

    int height = 0;
    for (unsigned int i = 0; i < 20; i++) {
        char msg[512];
        snprintf(msg, sizeof(msg), "%d: %s", i, rand() % 2 == 0 ? "Longer test message for alignment testing. Longer test message for alignment testing."
                    : "Short message.");
        height += add_old_chat_message(msg);
    }
    gtk_widget_set_size_request(stretchable_box_old_messages, -1, height);

    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    g_signal_connect(vadjustment, "value-changed", G_CALLBACK(on_scroll_value_changed), scrolled_window);

    gtk_widget_set_hexpand(stretchable_box_old_messages, TRUE);
    gtk_widget_set_hexpand(stretchable_box_new_messages, TRUE);
    GtkWidget *chat_box_bottom = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.helpers.set_classname_and_id(chat_box_bottom, "chat__bottom");
    gtk_widget_set_size_request(chat_box_bottom, -1, 72);
    gtk_box_pack_start(GTK_BOX(chat_box), chat_box_bottom, FALSE, FALSE, 0);


    g_idle_add(set_scroll_to_bottom, vadjustment);
    return chat_box;
}

t_chat init_chat(void) {
    t_chat chat = {
        .init = chat_init,
        .static_height = 0,
    };
    return chat;
}
