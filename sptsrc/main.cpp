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

int main(int argc, char *argv[])
{
	my_serial_ctrl *pS = new my_serial_ctrl();

	GtkBuilder *builder = NULL;
	GObject *window = NULL;
	GObject *comboBoxText = NULL;
	GError *error = NULL;

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
