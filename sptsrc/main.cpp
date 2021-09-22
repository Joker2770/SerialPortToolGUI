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
gchar g_Data_0[16] = "\0";
gchar g_Data_1[16] = "\0";
gchar g_Data_2[16] = "\0";
gchar g_Data_3[16] = "\0";
gchar g_Data_4[16] = "\0";

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
		gtk_widget_show_all (dialog);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
}

static void
open_callback(GtkWidget *widget, gpointer data)
{
	if (nullptr != pS)
	{
		pS->show_port_set();

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
	try
	{
		gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		printf("cbt port change: %s\n", gData);
		pS->m_serial->setPort(gData);
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
cbt_baudrate_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		printf("cbt baudrate change: %s\n", gData);
		pS->m_serial->setBaudrate(atol(gData));
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
cbt_bytesize_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		printf("cbt bytesize change: %s\n", gData);
		pS->m_serial->setBytesize((serial::bytesize_t)atoi(gData));
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
cbt_parity_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		printf("cbt parity change: %s\n", gData);
		if (0 == strcmp(gData, "none"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_none);
		else if (0 == strcmp(gData, "odd"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_odd);
		else if (0 == strcmp(gData, "even"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_even);
		else if (0 == strcmp(gData, "mark"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_mark);
		else if (0 == strcmp(gData, "space"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_space);
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
cbt_stopbits_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		printf("cbt stopbits change: %s\n", gData);
		if (0 == strcmp(gData, "1.5"))
			pS->m_serial->setStopbits((serial::stopbits_t)serial::stopbits_one_point_five);
		else
			pS->m_serial->setStopbits((serial::stopbits_t)atoi(gData));
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
cbt_flowcontrol_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
		printf("cbt flowcontrol change: %s\n", gData);
		if (0 == strcmp(gData, "none"))
			pS->m_serial->setFlowcontrol((serial::flowcontrol_t)serial::flowcontrol_none);
		else if (0 == strcmp(gData, "software"))
			pS->m_serial->setFlowcontrol((serial::flowcontrol_t)serial::flowcontrol_software);
		else if (0 == strcmp(gData, "hardware"))
			pS->m_serial->setFlowcontrol((serial::flowcontrol_t)serial::flowcontrol_hardware);
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void
dlg_to_entry_0_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_0, 0, sizeof(g_Data_0));
	strncpy(g_Data_0, (char*)gtk_entry_get_text(GTK_ENTRY(widget)), 15);
	printf("entry_0 change: %s\n", (char*)g_Data_0);
}

static void
dlg_to_entry_1_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_1, 0, sizeof(g_Data_1));
	strncpy(g_Data_1, (char*)gtk_entry_get_text(GTK_ENTRY(widget)), 15);
	printf("entry_1 change: %s\n", (char*)g_Data_1);
}

static void
dlg_to_entry_2_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_2, 0, sizeof(g_Data_2));
	strncpy(g_Data_2, (char*)gtk_entry_get_text(GTK_ENTRY(widget)), 15);
	printf("entry_2 change: %s\n", (char*)g_Data_2);
}

static void
dlg_to_entry_3_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_3, 0, sizeof(g_Data_3));
	strncpy(g_Data_3, (char*)gtk_entry_get_text(GTK_ENTRY(widget)), 15);
	printf("entry_3 change: %s\n", (char*)g_Data_3);
}

static void
dlg_to_entry_4_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_4, 0, sizeof(g_Data_4));
	strncpy(g_Data_4, (char*)gtk_entry_get_text(GTK_ENTRY(widget)), 15);
	printf("entry_4 change: %s\n", (char*)g_Data_4);
}

static void 
btn_timeout_setting_callback(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog, *label, *content_area, *grid, *entry;
	GtkDialogFlags flags;
	// Create the widgets
	flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	dialog = gtk_dialog_new_with_buttons ("timeout setting",
											GTK_WINDOW(data),
											flags,
											"apply",
											GTK_RESPONSE_NONE,
											NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	grid = gtk_grid_new();

	label = gtk_label_new ("inter_byte_timeout");
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "65535");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);
	memset(g_Data_0, 0, sizeof(g_Data_0));
	strcpy(g_Data_0, "65535");
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_0_callback), NULL);

	label = gtk_label_new ("read_timeout_constant");
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "1000");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
	memset(g_Data_1, 0, sizeof(g_Data_1));
	strcpy(g_Data_1, "1000");
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_1_callback), NULL);

	label = gtk_label_new ("read_timeout_multiplier");
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "0");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 2, 1, 1);
	memset(g_Data_2, 0, sizeof(g_Data_2));
	strcpy(g_Data_2, "0");
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_2_callback), NULL);

	label = gtk_label_new ("write_timeout_constant");
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "1000");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 3, 1, 1);
	memset(g_Data_3, 0, sizeof(g_Data_3));
	strcpy(g_Data_3, "1000");
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_3_callback), NULL);

	label = gtk_label_new ("write_timeout_multiplier");
	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "0");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 4, 1, 1);
	memset(g_Data_4, 0, sizeof(g_Data_4));
	strcpy(g_Data_4, "0");
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_4_callback), NULL);


	// Ensure that the dialog box is destroyed when the user responds
	g_signal_connect_swapped (dialog,
                           "response",
                           G_CALLBACK (gtk_widget_destroy),
                           dialog);

	// Add the label, and show everything we’ve added
	gtk_container_add (GTK_CONTAINER (content_area), grid);
	gtk_widget_show_all (dialog);
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
	case GTK_RESPONSE_NONE:
		try
		{
			printf("%s,%s,%s,%s,%s\n", g_Data_0, g_Data_1, g_Data_2, g_Data_3, g_Data_4);
			pS->m_serial->setTimeout(atol(g_Data_0), atol(g_Data_1), atol(g_Data_2), atol(g_Data_3), atol(g_Data_4));
		}
		catch (exception &e)
		{
			printf("Unhandled Exception: %s\n", e.what());
			show_errMsg(e.what(), data);
		}
		break;
	default:
		break;
	}
	gtk_widget_destroy(dialog);
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
	g_signal_connect(button, "clicked", G_CALLBACK(open_callback), (gpointer)window);

	button = gtk_builder_get_object(builder, "btn_close");
	g_signal_connect(button, "clicked", G_CALLBACK(close_callback), (gpointer)window);

	button = gtk_builder_get_object(builder, "btn_timeout_setting");
	g_signal_connect(button, "clicked", G_CALLBACK(btn_timeout_setting_callback), (gpointer)window);

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

	try
	{
		gchar* gc = NULL;

		//setBaudrate
		comboBoxText = gtk_builder_get_object(builder, "cbt_baudrate");
		pS->m_serial->setBaudrate(atol(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText))));
		g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_baudrate_callback), (gpointer)window);

		//setBytesize
		comboBoxText = gtk_builder_get_object(builder, "cbt_bytesize");
		pS->m_serial->setBytesize((serial::bytesize_t)atoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText))));
		g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_bytesize_callback), (gpointer)window);

		//setParity
		comboBoxText = gtk_builder_get_object(builder, "cbt_parity");
		gc = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText));
		if (0 == strcmp(gc, "none"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_none);
		else if (0 == strcmp(gc, "odd"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_odd);
		else if (0 == strcmp(gc, "even"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_even);
		else if (0 == strcmp(gc, "mark"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_mark);
		else if (0 == strcmp(gc, "space"))
			pS->m_serial->setParity((serial::parity_t)serial::parity_space);
		g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_parity_callback), (gpointer)window);

		//setStopbits
		comboBoxText = gtk_builder_get_object(builder, "cbt_stopbits");
		gc = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText));
		if (0 == strcmp(gc, "1.5"))
			pS->m_serial->setStopbits((serial::stopbits_t)serial::stopbits_one_point_five);
		else
			pS->m_serial->setStopbits((serial::stopbits_t)atoi(gc));
		g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_stopbits_callback), (gpointer)window);

		//setFlowcontrol
		comboBoxText = gtk_builder_get_object(builder, "cbt_flowcontrol");
		gc = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(comboBoxText));
		if (0 == strcmp(gc, "none"))
			pS->m_serial->setFlowcontrol((serial::flowcontrol_t)serial::flowcontrol_none);
		else if (0 == strcmp(gc, "software"))
			pS->m_serial->setFlowcontrol((serial::flowcontrol_t)serial::flowcontrol_software);
		else if (0 == strcmp(gc, "hardware"))
			pS->m_serial->setFlowcontrol((serial::flowcontrol_t)serial::flowcontrol_hardware);
		g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_flowcontrol_callback), (gpointer)window);
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), window);
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
