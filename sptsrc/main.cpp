/*************************************************************************
    > File Name: main.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Sun Feb 23 9:03:57 2020
 ************************************************************************/

/* 
    A Cross-platform serial debug tool.
    Copyright (C) 2021  joker2770(Jintao Yang)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "port_control.h"
#include <gtk/gtk.h>

my_serial_ctrl *pS = nullptr;

static void
show_errMsg(const gchar* errMsg, gpointer data)
{
		GtkWidget *dialog = nullptr;
		dialog = gtk_message_dialog_new(GTK_WINDOW(data),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_OK,
					"%s",errMsg);
		gtk_window_set_title(GTK_WINDOW(dialog), "Error");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
}

static void
open_callback(GtkWidget *widget, gpointer data)
{
	if (nullptr != pS)
	{
		gchar errMsg[256] = "";
		memset(errMsg, 0, sizeof(errMsg));
		int ret = pS->open_port(errMsg);
		printf("open ret: %d\n", ret);

		if (strlen(errMsg)>0)
			show_errMsg(errMsg, data);
	}
}

static void
close_callback(GtkWidget *widget, gpointer data)
{
	if (nullptr != pS)
	{
		gchar errMsg[128] = "";
		memset(errMsg, 0, sizeof(errMsg));
		int ret = pS->close_port(errMsg);
		printf("close ret: %d\n", ret);

		if (strlen(errMsg)>0)
			show_errMsg(errMsg, data);
	}
}

static void 
cbt_port_callback(GtkWidget *widget, gpointer data)
{
	gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	printf("cbt port change: %s\n", gData);
	pS->m_serial->setPort(gData);
}

static void 
cbt_baudrate_callback(GtkWidget *widget, gpointer data)
{
	gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	printf("cbt baudrate change: %s\n", gData);
	pS->m_serial->setBaudrate(atol(gData));
}

int main(int argc, char *argv[])
{
	pS = new my_serial_ctrl();

	GtkBuilder *builder = NULL;
	GObject *window = NULL;
	GObject *comboBoxText = NULL;
	GObject *button = NULL;
	GError *error = NULL;

	const gchar *entry_port = "/dev/ttyUSB0";
	const gchar *entry_baud = "57600";

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "SerialPortToolGUI.ui", &error) == 0)
	{
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		return 1;
	}

	window = gtk_builder_get_object(builder, "mainWindow");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	button = gtk_builder_get_object(builder, "btn_open");
	g_signal_connect(button, "clicked", G_CALLBACK(open_callback), NULL);

	button = gtk_builder_get_object(builder, "btn_close");
	g_signal_connect(button, "clicked", G_CALLBACK(close_callback), NULL);

	comboBoxText = gtk_builder_get_object(builder, "cbt_port");
	//entry_port = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText));
	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(comboBoxText));
	vector<serial::PortInfo> devices_found = serial::list_ports();
	vector<serial::PortInfo>::iterator iter = devices_found.begin();
	int i = 0;
	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;
		gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboBoxText), to_string(i).c_str(), device.port.c_str());
		i++;
		printf("%d. Port - <%s>\n\tDescription: %s\n\tHardware_id: %s\n\n", i, device.port.c_str(), device.description.c_str(), device.hardware_id.c_str());
	}
	g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_port_callback), (gpointer)window);

	//setBaudrate
	comboBoxText = gtk_builder_get_object(builder, "cbt_baudrate");
	pS->m_serial->setBaudrate(atol(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText))));
	g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_baudrate_callback), NULL);

	g_object_unref(builder);

	gtk_widget_show(GTK_WIDGET(window));
	gtk_main();

	if (NULL != pS)
	{
		delete pS;
		pS = NULL;
	}

	return 0;
}
