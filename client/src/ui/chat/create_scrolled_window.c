#include "../../../inc/header.h"

static int draw_chat(GtkWidget *message_wrapper, const char *message_txt, int is_received) {
    gtk_widget_set_halign(message_wrapper, is_received == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
    vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
    int width = 500;

    GtkWidget *message = gtk_overlay_new();
    gtk_widget_set_hexpand(message, FALSE);
    gtk_widget_set_halign(message, GTK_ALIGN_FILL);

    GtkWidget *event_box = gtk_event_box_new();
    vendor.helpers.add_hover(event_box);
    gtk_container_add(GTK_CONTAINER(event_box), message);

//    vendor.helpers.set_classname_and_id(message, "chat__message");
    vendor.helpers.set_classname_and_id(event_box, "chat__message");
    gtk_box_pack_start(GTK_BOX(message_wrapper), event_box, FALSE, FALSE, 0);

    GtkWidget *message_text = gtk_label_new(message_txt);
    PangoLayout *layout = gtk_label_get_layout(GTK_LABEL(message_text));

    int _width, _height;
    int add_to_height = 14;
    pango_layout_get_size(layout, &_width, &_height);
    int width_in_pixels = _width / PANGO_SCALE;
    if (width_in_pixels < 20) add_to_height += 12;;
    if (width_in_pixels < 40) {
        width_in_pixels = 40;

    }
    if (width_in_pixels < width) {
        width = width_in_pixels + 40;
    }

    pango_layout_set_width(layout, width * PANGO_SCALE);
    int line_count = pango_layout_get_line_count(layout);
    double magic_coefficient;
    if (line_count < 20) magic_coefficient = 22;
    else magic_coefficient = 17.87;
    int height = line_count * magic_coefficient + add_to_height;

    if (line_count > 0) {
        PangoLayoutLine *last_line = pango_layout_get_line(layout, line_count - 1);
        if (last_line) {
            PangoRectangle ink_rect;
            pango_layout_line_get_extents(last_line, &ink_rect, NULL);

            int last_line_width = ink_rect.width / PANGO_SCALE;

            if (last_line_width + 60 >= width) {
                height += 16;
            }
        }
    }

    gtk_widget_set_size_request(message, width, height);
    gtk_widget_set_size_request(message_text, width, height);
    gtk_widget_set_halign(message_text, GTK_ALIGN_START);
    gtk_style_context_add_class(gtk_widget_get_style_context(message_text), is_received ? "_received" : "_sended");
    gtk_label_set_line_wrap(GTK_LABEL(message_text), TRUE);
    vendor.helpers.set_classname_and_id(message_text, "chat__message__text");

    gtk_overlay_add_overlay(GTK_OVERLAY(message), message_text);

    GtkWidget *time = gtk_label_new("00:00");

    gtk_overlay_add_overlay(GTK_OVERLAY(message), time);
    vendor.helpers.set_classname_and_id(time, "chat__message__time");

    gtk_widget_set_margin_start(time, 420);
    gtk_widget_set_margin_bottom(time, 0);
    gtk_widget_set_halign(time, GTK_ALIGN_END);
    gtk_widget_set_valign(time, GTK_ALIGN_END);

    gtk_widget_show_all(message_wrapper);
    return height + 12;
}

void add_chat_message(const char *message_txt, int is_received) {
    GtkAdjustment *adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(vendor.pages.main_page.chat.scrolled_window));
    double upper = gtk_adjustment_get_upper(adjustment);
    double current_value = gtk_adjustment_get_value(adjustment);
    double page_size = gtk_adjustment_get_page_size(adjustment);

    GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(vendor.pages.main_page.chat.stretchable_box_new_messages), message_wrapper, FALSE, FALSE, 0);

    draw_chat(message_wrapper, message_txt, is_received);

    if (!is_received) {
        g_idle_add(set_scroll_to_bottom, adjustment);
        return;
    }

    if (current_value + page_size + 10 >= upper) {
        g_idle_add(set_scroll_to_bottom, adjustment);
    }

}

static int add_old_chat_message(const char *message_txt, int is_received) {
    GtkWidget *message_wrapper = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(message_wrapper, is_received == 0 ? GTK_ALIGN_END : GTK_ALIGN_START);
    vendor.helpers.set_classname_and_id(message_wrapper, "chat__message__wrapper");
    gtk_box_pack_end(GTK_BOX(vendor.pages.main_page.chat.stretchable_box_old_messages), message_wrapper, FALSE, FALSE, 0);

    return draw_chat(message_wrapper, message_txt, is_received);
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
    	t_messages_struct *messages = vendor.database.tables.messages_table.get_messages_by_chat_id(1, PER_PAGE,
			++vendor.pages.main_page.chat.page, &vendor.pages.main_page.chat.total_messages);

    	if (messages != NULL) {
        	for (int i = 0; i < messages_count; i++) {
				int is_received = messages[i].sender_struct->user_id != vendor.current_user.user_id;
            	height += add_old_chat_message(messages[i].message_text, is_received);
            	vendor.pages.main_page.chat.shown_messages++;
			}

			vendor.database.tables.messages_table.free_struct(messages);
		}

        GtkAdjustment *v_adjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolled_window));
        t_scroll_data *scroll_data = g_malloc(sizeof(t_scroll_data));
        scroll_data->adjustment = v_adjustment;
        scroll_data->height = height;

        g_idle_add(scroll_to_height, scroll_data);
    }
}

GtkWidget *chat_create_scrolled_window(void) {
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    vendor.pages.main_page.chat.scrolled_window = scrolled_window;
    vendor.helpers.set_classname_and_id(scrolled_window, "chat__scrolled-window");

    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
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
    t_messages_struct *messages = vendor.database.tables.messages_table.get_messages_by_chat_id(1, PER_PAGE,
			++vendor.pages.main_page.chat.page, &vendor.pages.main_page.chat.total_messages);

    if (messages != NULL) {
        for (int i = 0; i < PER_PAGE; i++) {
        	if ((i + 1) * vendor.pages.main_page.chat.page > vendor.pages.main_page.chat.total_messages) {
				break;
            }
			int is_received = messages[i].sender_struct->user_id != vendor.current_user.user_id;
//			g_print("messages[i].sender_struct->user_id: %d\n", messages[i].sender_struct->user_id);
//			g_print("vendor.current_user.user_id: %d\n\n", vendor.current_user.user_id);

            height += add_old_chat_message(messages[i].message_text, is_received);
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
