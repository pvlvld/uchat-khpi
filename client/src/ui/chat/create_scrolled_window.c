#include "../../../inc/header.h"

static void on_sender_event_box_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    (void) widget;
    (void) event;
    t_users_struct *sender_struct = (t_users_struct *) user_data;

    int chat_id = vendor.database.tables.personal_chats_table.get_personal_chat_with_user(sender_struct->user_id);
    if (chat_id != 0) {
        activate_chatblock_by_id(chat_id);
    }
}

static GtkWidget *create_sender_event_box(t_messages_struct *message_struct, int *add_to_height, int *min_width, int width) {
    GtkWidget *sender_name = gtk_label_new(message_struct->sender_struct->username);
    gtk_label_set_lines(GTK_LABEL(sender_name), 1);
    gtk_label_set_ellipsize(GTK_LABEL(sender_name), PANGO_ELLIPSIZE_END);
    vendor.helpers.set_classname_and_id(sender_name, "chat__message_sender__text");

    GtkWidget *sender_event_box = gtk_event_box_new();
    vendor.helpers.add_hover(sender_event_box);
    gtk_container_add(GTK_CONTAINER(sender_event_box), sender_name);
    gtk_widget_set_margin_start(sender_event_box, 10);
    gtk_widget_set_margin_top(sender_event_box, 12);
    gtk_widget_set_halign(sender_event_box, GTK_ALIGN_START);
    gtk_widget_set_valign(sender_event_box, GTK_ALIGN_START);
    (*add_to_height) = 20;

    PangoLayout *layout = gtk_widget_create_pango_layout(sender_name, message_struct->sender_struct->username);
    pango_layout_set_text(layout, message_struct->sender_struct->username, -1);
    pango_layout_get_size(layout, min_width, NULL);
    (*min_width) /= PANGO_SCALE;

    gtk_widget_set_size_request(sender_name, (*min_width) + 40 < width ? (*min_width) : width - 40, -1);
    g_object_unref(layout);

    g_signal_connect(sender_event_box, "button-press-event", G_CALLBACK(on_sender_event_box_click), message_struct->sender_struct);
    return sender_event_box;
}

static int draw_chat(GtkWidget *message_wrapper, t_messages_struct *message_struct, int is_received, int is_new) {
    gtk_widget_set_halign(message_wrapper, is_received == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
    vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
    int width = 500;
    int min_width = 0;
    int add_to_height = 0;
    int height = 50;

    GtkWidget *message = gtk_overlay_new();
    gtk_widget_set_hexpand(message, FALSE);
    gtk_widget_set_halign(message, GTK_ALIGN_START);
    gtk_widget_set_valign(message, GTK_ALIGN_START);

    GtkWidget *event_box = gtk_event_box_new();
    vendor.helpers.add_hover(event_box);
    gtk_container_add(GTK_CONTAINER(event_box), message);

    vendor.helpers.set_classname_and_id(event_box, "chat__message");
    char *message_text_txt = message_struct->message_text;
    if (!g_utf8_validate(message_text_txt, -1, NULL)) {
        g_warning("Invalid UTF-8 detected!");
    }

    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_family(font_desc, "Ubuntu Sans");
    pango_font_description_set_size(font_desc, 12.2 * PANGO_SCALE);

    if (is_received == 1 && vendor.active_chat.chat->type != 0) {
        GtkWidget *avatar_event = gtk_event_box_new();
        GtkWidget *drawing_area = vendor.helpers.create_avatar(message_struct->sender_struct->username, 50, 50);
        gtk_container_add(GTK_CONTAINER(avatar_event), drawing_area);
        gtk_box_pack_start(GTK_BOX(message_wrapper), avatar_event, FALSE, FALSE, 4);
        vendor.helpers.add_hover(avatar_event);
        g_signal_connect(avatar_event, "button-press-event", G_CALLBACK(on_sender_event_box_click), message_struct->sender_struct);

        GtkWidget *sender_event_box = create_sender_event_box(message_struct, &add_to_height, &min_width, width);
        gtk_overlay_add_overlay(GTK_OVERLAY(message), sender_event_box);
        vendor.helpers.set_classname_and_id(sender_event_box, "chat__message_sender");
    }

    gtk_box_pack_start(GTK_BOX(message_wrapper), event_box, FALSE, FALSE, 0);

    if (message_struct->path_to_image == NULL) {
        t_message_info_struct *message_info_struct = g_new(t_message_info_struct, 1);
        message_info_struct->widget = message_wrapper;
        message_info_struct->chat_id = message_struct->chat_struct->chat_id;
        message_info_struct->message_id = message_struct->message_id;
        message_info_struct->is_new = is_new;
        message_info_struct->sender_id = message_struct->sender_struct->user_id;

        GtkWidget *message_text = create_message_box(message_text_txt, message_info_struct);

        PangoLayout *layout = gtk_widget_create_pango_layout(message_text, message_struct->message_text);
        pango_layout_set_text(layout, message_text_txt, -1);

        pango_layout_set_font_description(layout, font_desc);
        pango_font_description_free(font_desc);

        pango_layout_set_auto_dir(layout, FALSE);
        int _width;
        pango_layout_get_size(layout, &_width, NULL);
        int width_in_pixels = _width / PANGO_SCALE;

        if (width_in_pixels < 40) {
            width_in_pixels = 40;
        }
        if (min_width > width_in_pixels) {
            width_in_pixels = min_width;
        }

        if (width_in_pixels < width) {
            width = width_in_pixels + 40;
        }

        pango_layout_set_width(layout, width * PANGO_SCALE);
        int _height;
        pango_layout_get_size(layout, NULL, &_height);
        g_object_unref(layout);

        int height_in_pixels = _height / PANGO_SCALE;
        height = height_in_pixels + 32 + add_to_height;

        gtk_widget_set_size_request(message, width, height);
        gtk_widget_set_size_request(message_text, width, height - add_to_height);
        gtk_widget_set_halign(message_text, GTK_ALIGN_START);
        gtk_style_context_add_class(gtk_widget_get_style_context(message_text), is_received ? "_received" : "_sended");
        if (add_to_height != 0) {
            gtk_style_context_add_class(gtk_widget_get_style_context(message_text), "_top");
        }

        gtk_overlay_add_overlay(GTK_OVERLAY(message), message_text);
        gtk_overlay_reorder_overlay(GTK_OVERLAY(message), message_text, 0);

        height += 12;
    } else {
        GtkWidget *message_image_event = gtk_event_box_new();
        vendor.helpers.set_classname_and_id(message_image_event, "message__image__wrapper");

        GtkWidget *message_image_drawing_area = vendor.helpers.create_image(message_struct->path_to_image, message, 500);
        gtk_widget_set_margin_top(message_image_drawing_area, 10);

        gtk_widget_set_halign(message_image_drawing_area, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(message_image_drawing_area, GTK_ALIGN_START);

        gtk_container_add(GTK_CONTAINER(message_image_event), message_image_drawing_area);

        vendor.helpers.add_hover(message_image_event);

        gtk_overlay_add_overlay(GTK_OVERLAY(message), message_image_event);
        gtk_overlay_reorder_overlay(GTK_OVERLAY(message), message_image_event, 0);
    }


    GtkWidget *time = gtk_label_new(format_timestamp(message_struct->timestamp));

    gtk_overlay_add_overlay(GTK_OVERLAY(message), time);
    vendor.helpers.set_classname_and_id(time, "chat__message__time");

    gtk_widget_set_margin_start(time, 420);
    gtk_widget_set_margin_bottom(time, 0);
    gtk_widget_set_halign(time, GTK_ALIGN_END);
    gtk_widget_set_valign(time, GTK_ALIGN_END);

    return height;
}

static void clear_widget(GtkWidget *container) {
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));

    for (GList *child = children; child != NULL; child = child->next) {
        gtk_widget_destroy(GTK_WIDGET(child->data));
    }

    g_list_free(children);
}


void redraw_message_wrapper(t_message_info_struct *info, t_messages_struct *message_struct) {
    clear_widget(info->widget);
    draw_chat(info->widget, message_struct, 0, 0);

    gtk_widget_queue_resize(info->widget);
    gtk_widget_show_all(info->widget);

    GtkWidget *stretchable_box = info->is_new ? vendor.pages.main_page.chat.stretchable_box_new_messages : vendor.pages.main_page.chat.stretchable_box_old_messages;

    int height = gtk_widget_get_allocated_height(info->widget);
    int content_box_height = gtk_widget_get_allocated_height(stretchable_box);

    gtk_widget_set_size_request(stretchable_box, -1, content_box_height - height);
}

void add_chat_message(t_messages_struct *message, int is_received) {
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(vendor.pages.main_page.chat.scrolled_window));
    double upper = gtk_adjustment_get_upper(adjustment);
    double current_value = gtk_adjustment_get_value(adjustment);
    double page_size = gtk_adjustment_get_page_size(adjustment);

    GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(vendor.pages.main_page.chat.stretchable_box_new_messages), message_wrapper, FALSE, FALSE, 0);

    draw_chat(message_wrapper, message, is_received, 1);
    gtk_widget_show_all(message_wrapper);

    if (!is_received) {
        g_idle_add(set_scroll_to_bottom, adjustment);
        return;
    }

    if (current_value + page_size + 10 >= upper) {
        g_idle_add(set_scroll_to_bottom, adjustment);
    }

    gtk_widget_show(vendor.active_chat.chat_sidebar_widget);
}

static int add_old_chat_message(t_messages_struct *message, int is_received) {
    GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_halign(message_wrapper, is_received == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
    vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
    gtk_box_pack_end(GTK_BOX(vendor.pages.main_page.chat.stretchable_box_old_messages), message_wrapper, FALSE, FALSE, 0);

    return draw_chat(message_wrapper, message, is_received, 0);
}

static gboolean scroll_to_height(gpointer data) {
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
        int dif = vendor.pages.main_page.chat.total_messages - vendor.pages.main_page.chat.page * PER_PAGE;
        int messages_count = dif >= PER_PAGE ? PER_PAGE : dif;
	if (messages_count <= 0) return;
    	t_messages_struct *messages = vendor.database.tables.messages_table.get_messages_by_chat_id(vendor.active_chat.chat->id, PER_PAGE,
			++vendor.pages.main_page.chat.page, &vendor.pages.main_page.chat.total_messages);

    	if (messages != NULL) {
            for (int i = 0; i < messages_count; i++) {
	        	int is_received = messages[i].sender_struct->user_id != vendor.current_user.user_id;
            	height += add_old_chat_message(&messages[i], is_received);
            	vendor.pages.main_page.chat.shown_messages++;
            }
            vendor.database.tables.messages_table.free_struct(messages);
        }

        GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
        t_scroll_data *scroll_data = g_malloc(sizeof(t_scroll_data));
        scroll_data->adjustment = v_adjustment;
        scroll_data->height = height;

        int _height = gtk_widget_get_allocated_height(vendor.pages.main_page.chat.stretchable_box_old_messages);
        gtk_widget_set_size_request(vendor.pages.main_page.chat.stretchable_box_old_messages, -1, _height + height);
        g_idle_add(scroll_to_height, scroll_data);

        g_timeout_add(300, (GSourceFunc)gtk_widget_show_all, vendor.pages.main_page.chat.stretchable_box_old_messages);
    }
}

GtkWidget *chat_create_scrolled_window(void) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.pages.main_page.chat.scrolled_window = scrolled_window;
    vendor.helpers.set_classname_and_id(scrolled_window, "chat__scrolled-window");

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    vendor.pages.main_page.chat.content_box = content_box;
    gtk_container_add(GTK_CONTAINER(scrolled_window), content_box);
    gtk_widget_set_valign(content_box, GTK_ALIGN_END);

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
    t_messages_struct *messages = vendor.database.tables.messages_table.get_messages_by_chat_id(vendor.active_chat.chat->id, PER_PAGE,
			++vendor.pages.main_page.chat.page, &vendor.pages.main_page.chat.total_messages);

    if (messages != NULL) {
        for (int i = 0; i < PER_PAGE; i++) {
            if ((i + 1) * vendor.pages.main_page.chat.page > vendor.pages.main_page.chat.total_messages) {
                break;
            }
            int is_received = messages[i].sender_struct->user_id != vendor.current_user.user_id;

            height += add_old_chat_message(&messages[i], is_received);
            vendor.pages.main_page.chat.shown_messages++;
        }
	vendor.database.tables.messages_table.free_struct(messages);
    }

    gtk_widget_set_size_request(stretchable_box_old_messages, -1, height);

    GtkAdjustment *vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
    vendor.pages.main_page.chat.vadjustment = vadjustment;
    g_signal_connect(vadjustment, "value-changed", G_CALLBACK(on_scroll_value_changed), scrolled_window);

    gtk_widget_set_hexpand(stretchable_box_old_messages, TRUE);
    gtk_widget_set_hexpand(stretchable_box_new_messages, TRUE);

    g_idle_add(set_scroll_to_bottom, vadjustment);

    return scrolled_window;
}
