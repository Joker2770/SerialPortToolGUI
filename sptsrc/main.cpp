/*************************************************************************
    > File Name: main.cpp
    > Author: Jintao Yang
    > Mail: 18608842770@163.com 
    > Created Time: Sun Feb 23 9:03:57 2020
 ************************************************************************/

/* 
    A Cross-platform serial debug tool.
    Copyright (C) 2021-2023  joker2770(Jintao Yang)

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
#include "utility.h"
#include <gtk/gtk.h>
#include <time.h>

#define MAX_SEND (1024*100)

typedef enum
{
	crc4_itu = 0,
	crc5_epc = 1,
	crc5_usb = 2,
	crc5_itu = 3,
	crc6_itu = 4,
	crc7_mmc = 5,
	crc8 = 6,
	crc8_itu = 7,
	crc8_rohc = 8,
	crc8_maxim = 9,
	crc16_ccitt = 10,
	crc16_ccitt_false = 11,
	crc16_xmodem = 12,
	crc16_x25 = 13,
	crc16_modbus = 14,
	crc16_ibm = 15,
	crc16_maxim = 16,
	crc16_usb = 17,
	crc16_dnp = 18,
	crc32 = 19,
	crc32_mpeg = 20
} crc_alg_t;

my_serial_ctrl *pS = nullptr;
gchar g_data_buf_len[8] = "64";
gchar g_Data_0[16] = "\0";
gchar g_Data_1[16] = "\0";
gchar g_Data_2[16] = "\0";
gchar g_Data_3[16] = "\0";
gchar g_Data_4[16] = "\0";
gchar *g_text2send = NULL;
gchar *g_crc_in = NULL;
gboolean g_hex_output_checked = FALSE;
gboolean g_hex_send_checked = FALSE;
gboolean g_bRTS = FALSE;
gboolean g_bDTR = FALSE;
gboolean g_bBreak = FALSE;
unsigned int g_crc_flag = 6;

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
		g_free(gData);
		gData = NULL;
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
		g_free(gData);
		gData = NULL;
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
		g_free(gData);
		gData = NULL;
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
		g_free(gData);
		gData = NULL;
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
		g_free(gData);
		gData = NULL;
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
		g_free(gData);
		gData = NULL;
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void
cbt_crc_select_callback(GtkWidget *widget, gpointer data)
{
	gchar *gData = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
	printf("crc alg change: %s\n", gData);

	if (0 == strcmp(gData, "CRC4_ITU"))
		g_crc_flag = crc_alg_t::crc4_itu;
	else if (0 == strcmp(gData, "CRC5_EPC"))
		g_crc_flag = crc_alg_t::crc5_epc;
	else if (0 == strcmp(gData, "CRC5_USB"))
		g_crc_flag = crc_alg_t::crc5_usb;
	else if (0 == strcmp(gData, "CRC5_ITU"))
		g_crc_flag = crc_alg_t::crc5_itu;
	else if (0 == strcmp(gData, "CRC6_ITU"))
		g_crc_flag = crc_alg_t::crc6_itu;
	else if (0 == strcmp(gData, "CRC7_MMC"))
		g_crc_flag = crc_alg_t::crc7_mmc;
	else if (0 == strcmp(gData, "CRC8"))
		g_crc_flag = crc_alg_t::crc8;
	else if (0 == strcmp(gData, "CRC8_ITU"))
		g_crc_flag = crc_alg_t::crc8_itu;
	else if (0 == strcmp(gData, "CRC8_ROHC"))
		g_crc_flag = crc_alg_t::crc8_rohc;
	else if (0 == strcmp(gData, "CRC8_MAXIM"))
		g_crc_flag = crc_alg_t::crc8_maxim;
	else if (0 == strcmp(gData, "CRC16_CCITT"))
		g_crc_flag = crc_alg_t::crc16_ccitt;
	else if (0 == strcmp(gData, "CRC16_CCITT_FALSE"))
		g_crc_flag = crc_alg_t::crc16_ccitt_false;
	else if (0 == strcmp(gData, "CRC16_XMODEM"))
		g_crc_flag = crc_alg_t::crc16_xmodem;
	else if (0 == strcmp(gData, "CRC16_X25"))
		g_crc_flag = crc_alg_t::crc16_x25;
	else if (0 == strcmp(gData, "CRC16_MODBUS"))
		g_crc_flag = crc_alg_t::crc16_modbus;
	else if (0 == strcmp(gData, "CRC16_IBM"))
		g_crc_flag = crc_alg_t::crc16_ibm;
	else if (0 == strcmp(gData, "CRC16_MAXIM"))
		g_crc_flag = crc_alg_t::crc16_maxim;
	else if (0 == strcmp(gData, "CRC16_USB"))
		g_crc_flag = crc_alg_t::crc16_usb;
	else if (0 == strcmp(gData, "CRC16_DNP"))
		g_crc_flag = crc_alg_t::crc16_dnp;
	else if (0 == strcmp(gData, "CRC32"))
		g_crc_flag = crc_alg_t::crc32;
	else if (0 == strcmp(gData, "CRC32_MPEG"))
		g_crc_flag = crc_alg_t::crc32_mpeg;

	if (gData != NULL)
	{
		g_free(gData);
		gData = NULL;
	}
}

static void
entry_callback(GtkWidget *widget, gpointer data)
{
	memset(g_data_buf_len, 0, sizeof(g_data_buf_len));
	strcpy(g_data_buf_len, gtk_entry_get_text(GTK_ENTRY(widget)));
	g_print("entry buf len change: %s\n", (char*)g_data_buf_len);
}

static void
dlg_to_entry_0_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_0, 0, sizeof(g_Data_0));
	strcpy(g_Data_0, gtk_entry_get_text(GTK_ENTRY(widget)));
	g_print("entry_0 change: %s\n", (char*)g_Data_0);
}

static void
dlg_to_entry_1_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_1, 0, sizeof(g_Data_1));
	strcpy(g_Data_1, gtk_entry_get_text(GTK_ENTRY(widget)));
	g_print("entry_1 change: %s\n", (char*)g_Data_1);
}

static void
dlg_to_entry_2_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_2, 0, sizeof(g_Data_2));
	strcpy(g_Data_2, gtk_entry_get_text(GTK_ENTRY(widget)));
	g_print("entry_2 change: %s\n", (char*)g_Data_2);
}

static void
dlg_to_entry_3_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_3, 0, sizeof(g_Data_3));
	strcpy(g_Data_3, gtk_entry_get_text(GTK_ENTRY(widget)));
	g_print("entry_3 change: %s\n", (char*)g_Data_3);
}

static void
dlg_to_entry_4_callback(GtkWidget *widget, gpointer data)
{
	memset(g_Data_4, 0, sizeof(g_Data_4));
	strcpy(g_Data_4, gtk_entry_get_text(GTK_ENTRY(widget)));
	g_print("entry_4 change: %s\n", (char*)g_Data_4);
}

static void 
dlg_ms_cbt_0_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		if (strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget)), "FALSE") == 0)
		{
			pS->m_serial->setRTS(false);
			g_bRTS = FALSE;
		}
		else
		{
			pS->m_serial->setRTS(true);
			g_bRTS = TRUE;
		}
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
dlg_ms_cbt_1_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		if (strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget)), "FALSE") == 0)
		{
			pS->m_serial->setDTR(false);
			g_bDTR = FALSE;
		}
		else
		{
			pS->m_serial->setDTR(true);
			g_bDTR = TRUE;
		}
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
dlg_ms_cbt_2_callback(GtkWidget *widget, gpointer data)
{
	try
	{
		if (strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget)), "FALSE") == 0)
		{
			pS->m_serial->setBreak(false);
			g_bBreak = FALSE;
		}
		else
		{
			pS->m_serial->setBreak(true);
			g_bBreak = TRUE;
		}
	}
	catch (exception &e)
	{
		printf("Unhandled Exception: %s\n", e.what());
		show_errMsg(e.what(), data);
	}
}

static void 
btn_more_setting_callback(GtkWidget *widget, gpointer data)
{
	GtkWidget *dialog, *label, *content_area, *grid, *comboBoxText;
	GtkDialogFlags flags;
	// Create the widgets
	flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	dialog = gtk_dialog_new_with_buttons ("more setting",
											GTK_WINDOW(data),
											flags,
											NULL,
											NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	grid = gtk_grid_new();

	label = gtk_label_new ("RTS");
	comboBoxText = gtk_combo_box_text_new();
	gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(comboBoxText), "0", "FALSE");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboBoxText), "1", "TRUE");
	if (!g_bRTS)
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(comboBoxText)), "0");
	else
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(comboBoxText)), "1");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), comboBoxText, 1, 0, 1, 1);
	g_signal_connect(comboBoxText, "changed", G_CALLBACK (dlg_ms_cbt_0_callback), data);

	label = gtk_label_new ("DTR");
	comboBoxText = gtk_combo_box_text_new();
	gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(comboBoxText), "0", "FALSE");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboBoxText), "1", "TRUE");
	if (!g_bDTR)
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(comboBoxText)), "0");
	else
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(comboBoxText)), "1");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), comboBoxText, 1, 1, 1, 1);
	g_signal_connect(comboBoxText, "changed", G_CALLBACK (dlg_ms_cbt_1_callback), data);

	label = gtk_label_new ("break");
	comboBoxText = gtk_combo_box_text_new();
	gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(comboBoxText), "0", "FALSE");
	gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(comboBoxText), "1", "TRUE");
	if (!g_bBreak)
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(comboBoxText)), "0");
	else
		gtk_combo_box_set_active_id(GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(comboBoxText)), "1");
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), comboBoxText, 1, 2, 1, 1);
	g_signal_connect(comboBoxText, "changed", G_CALLBACK (dlg_ms_cbt_2_callback), data);

	// Add the label, and show everything we’ve added
	gtk_container_add (GTK_CONTAINER (content_area), grid);

	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

	// Ensure that the dialog box is destroyed when the user responds
	g_signal_connect_swapped (dialog,
                           "response",
                           G_CALLBACK (gtk_widget_destroy),
                           dialog);

	gtk_widget_destroy(dialog);
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
											GTK_RESPONSE_OK,
											NULL);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog),GTK_RESPONSE_OK);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	grid = gtk_grid_new();

	label = gtk_label_new ("inter_byte_timeout");
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 8);
	if (g_Data_0[0] == '\0')
	{
		memset(g_Data_0, 0, sizeof(g_Data_0));
		strcpy(g_Data_0, "65535");
	}
	gtk_entry_set_text(GTK_ENTRY(entry), g_Data_0);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_0_callback), NULL);

	label = gtk_label_new ("read_timeout_constant");
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 8);
	if (g_Data_1[0] == '\0')
	{
		memset(g_Data_1, 0, sizeof(g_Data_1));
		strcpy(g_Data_1, "1000");
	}
	gtk_entry_set_text(GTK_ENTRY(entry), g_Data_1);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_1_callback), NULL);

	label = gtk_label_new ("read_timeout_multiplier");
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 8);
	if (g_Data_2[0] == '\0')
	{
		memset(g_Data_2, 0, sizeof(g_Data_2));
		strcpy(g_Data_2, "0");
	}
	gtk_entry_set_text(GTK_ENTRY(entry), g_Data_2);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 2, 1, 1);
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_2_callback), NULL);

	label = gtk_label_new ("write_timeout_constant");
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 8);
	if (g_Data_3[0] == '\0')
	{
		memset(g_Data_3, 0, sizeof(g_Data_3));
		strcpy(g_Data_3, "1000");
	}
	gtk_entry_set_text(GTK_ENTRY(entry), g_Data_3);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 3, 1, 1);
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_3_callback), NULL);

	label = gtk_label_new ("write_timeout_multiplier");
	entry = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(entry), 8);
	if (g_Data_4[0] == '\0')
	{
		memset(g_Data_4, 0, sizeof(g_Data_4));
		strcpy(g_Data_4, "0");
	}
	gtk_entry_set_text(GTK_ENTRY(entry), g_Data_4);
	gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), entry, 1, 4, 1, 1);
	g_signal_connect(entry, "changed", G_CALLBACK (dlg_to_entry_4_callback), NULL);

	// Add the label, and show everything we’ve added
	gtk_container_add (GTK_CONTAINER (content_area), grid);
	gtk_widget_show_all (dialog);
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch(result)
	{
	case GTK_RESPONSE_OK:
		try
		{
			g_print("%s,%s,%s,%s,%s\n", g_Data_0, g_Data_1, g_Data_2, g_Data_3, g_Data_4);
			pS->m_serial->setTimeout(atol(g_Data_0), atol(g_Data_1), atol(g_Data_2), atol(g_Data_3), atol(g_Data_4));
		}
		catch (exception &e)
		{
			g_print("Unhandled Exception: %s\n", e.what());
			show_errMsg(e.what(), data);
		}
		break;
	default:
		break;
	}

	// Ensure that the dialog box is destroyed when the user responds
	g_signal_connect_swapped (dialog,
                           "response",
                           G_CALLBACK (gtk_widget_destroy),
                           dialog);

	gtk_widget_destroy(dialog);
}

static void
chk_btn_output_callback(GtkWidget *widget, gpointer data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		g_hex_output_checked = TRUE;
		g_print("hex output checked\n");
	}
	else
	{
		g_hex_output_checked = FALSE;
		g_print("hex output released\n");
	}
}

static void
chk_btn_send_callback(GtkWidget *widget, gpointer data)
{
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
	{
		g_hex_send_checked = TRUE;
		g_print("hex send checked\n");
	}
	else
	{
		g_hex_send_checked = FALSE;
		g_print("hex send released\n");
	}
}

static void
text_view_send_callback(GtkWidget *widget, gpointer data)
{
	gchar* text = NULL;
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(widget), &start, &end);

	const GtkTextIter s = start, e = end;
	text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(widget), &s, &e, TRUE);

	g_print("text view(send):%s\n", text);

	if (NULL != g_text2send && strlen(text) < MAX_SEND)
	{
		memset(g_text2send, 0, MAX_SEND*sizeof(gchar));
		memcpy(g_text2send, (gchar*)text, strlen(text)*sizeof(gchar));
	}
	g_free(text);
	text = NULL;
}

static void
text_view_crc_in_callback(GtkWidget *widget, gpointer data)
{
	gchar* text = NULL;
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(widget), &start, &end);

	const GtkTextIter s = start, e = end;
	text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(widget), &s, &e, TRUE);

	g_print("text view(crc16):%s\n", text);

	if (NULL != g_crc_in && strlen(text) < MAX_SEND)
	{
		memset(g_crc_in, 0, MAX_SEND*sizeof(gchar));
		memcpy(g_crc_in, (gchar*)text, strlen(text)*sizeof(gchar));
	}
	g_free(text);
	text = NULL;
}

static void 
text_view_output_callback(GtkWidget *widget, gpointer data)
{
	if (NULL != pS)
	{
		time_t tt = time(NULL);
		char tTmp[32] = "\0";
		tm* t = localtime(&tt);
		sprintf(tTmp, "%02d:%02d:%02d ", t->tm_hour, t->tm_min, t->tm_sec);

		gchar errMsg_s[256] = "\0";
		gchar g_out[1024*100*4] = "\0";
		if (g_hex_send_checked)
		{
			string s2s = string(g_text2send);
			s2s = trimString(s2s);
			pS->send_data(s2s.c_str(), errMsg_s, g_hex_send_checked);
		}
		else
		{
			pS->send_data(g_text2send, errMsg_s, g_hex_send_checked);
		}

		strcat(g_out, tTmp);
		strcat(g_out, ">>");
		strcat(g_out, g_text2send);
		strcat(g_out, "\n");

		if (strlen(errMsg_s)>0)
		{
			strcat(g_out, errMsg_s);
			strcat(g_out, "\n");
		}

		// if (0 == i_ret_s)
		// {
		// 	gchar errMsg_r[256] = "\0";
		// 	char szRecieve[1024*100] = "";
		// 	i_ret_r = pS->receive_data(atol(g_data_buf_len), szRecieve, errMsg_r, g_hex_output_checked);

		// 	if (strlen(errMsg_r) > 0)
		// 	{
		// 		strcat(g_out, errMsg_r);
		// 		strcat(g_out, "\n");
		// 	}
		// 	else
		// 	{
		// 		if (g_hex_output_checked)
		// 		{
		// 			string sTmp = insert_space_split_2(szRecieve);
		// 			strcat(g_out, "<<");
		// 			strcat(g_out, sTmp.c_str());
		// 			strcat(g_out, "\n");
		// 		}
		// 		else
		// 		{
		// 			strcat(g_out, "<<");
		// 			strcat(g_out, szRecieve);
		// 			strcat(g_out, "\n");
		// 		}
		// 	}
		// }

		GtkTextIter start,end;
		gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(data),&start,&end);
		gtk_text_buffer_insert(GTK_TEXT_BUFFER(data), &end, g_out, strlen(g_out));
	}
}

static void 
scroll_output_callback(GtkWidget *widget, gpointer data)
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(widget),&start,&end);

	gtk_text_view_scroll_to_iter((GtkTextView*)data, &end, 0.0, TRUE, 0.99, 0.99);
}

static void 
tv_crc_callback(GtkWidget *widget, gpointer data)
{
	gchar g_out[16] = "\0";
	memset(g_out, 0, sizeof(g_out));

	if (g_crc_in[0] != '\0')
	{
		string s2s = string(g_crc_in);
		s2s = trimString(s2s);

		unsigned char szDest[1024 * 100] = "";
		memset(szDest, 0, sizeof(szDest));
		uint32_t ilen = 0;
		int iret = StringToHex((char *)s2s.c_str(), szDest, &ilen);

		if (iret == 0)
		{
			unsigned char crc_8 = 0;
			unsigned short crc_16 = 0;
			unsigned int crc_32 = 0;

			switch (g_crc_flag)
			{
			case crc_alg_t::crc4_itu:
				crc_8 = CRC4_ITU(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc5_epc:
				crc_8 = CRC5_EPC(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc5_usb:
				crc_8 = CRC5_USB(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc5_itu:
				crc_8 = CRC5_ITU(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc6_itu:
				crc_8 = CRC6_ITU(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc7_mmc:
				crc_8 = CRC7_MMC(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc8:
				crc_8 = CRC8(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc8_itu:
				crc_8 = CRC8_ITU(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc8_rohc:
				crc_8 = CRC8_ROHC(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc8_maxim:
				crc_8 = CRC8_MAXIM(szDest, ilen);
				sprintf(g_out, "%02x ", (crc_8 & 0xff));
				break;
			case crc_alg_t::crc16_ccitt:
				crc_16 = CRC16_CCITT(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_ccitt_false:
				crc_16 = CRC16_CCITT_FALSE(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_xmodem:
				crc_16 = CRC16_XMODEM(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_x25:
				crc_16 = CRC16_X25(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_modbus:
				crc_16 = CRC16_MODBUS(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_ibm:
				crc_16 = CRC16_IBM(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_maxim:
				crc_16 = CRC16_MAXIM(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_usb:
				crc_16 = CRC16_USB(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc16_dnp:
				crc_16 = CRC16_DNP(szDest, ilen);
				sprintf(g_out, "%02x %02x ", (crc_16 & 0xffff) >> 8, (crc_16 & 0x00ff));
				break;
			case crc_alg_t::crc32:
				crc_32 = CRC32(szDest, ilen);
				sprintf(g_out, "%02x %02x %02x %02x ", (crc_32 & 0xff000000) >> 24, (crc_32 & 0x00ff0000) >> 16, (crc_32 & 0x0000ff00) >> 8, (crc_32 & 0x000000ff));
				break;
			case crc_alg_t::crc32_mpeg:
				crc_32 = CRC32_MPEG(szDest, ilen);
				sprintf(g_out, "%02x %02x %02x %02x ", (crc_32 & 0xff000000) >> 24, (crc_32 & 0x00ff0000) >> 16, (crc_32 & 0x0000ff00) >> 8, (crc_32 & 0x000000ff));
				break;
			default:
				break;
			}
		}
	}

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(data), g_out, strlen(g_out));
}

static void 
clear_callback(GtkWidget *widget, gpointer data)
{
	GtkTextIter start,end;
	gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(data), &start, &end);
	gtk_text_buffer_delete(GTK_TEXT_BUFFER(data), &start,&end);
}

gboolean readDaemon(gpointer data)
{
	// clock_t clk_start, clk_end;
	// clk_start = clock();

	// uint32_t u_timeout = pS->m_serial->getTimeout().read_timeout_constant;

	if (pS->m_serial->isOpen())
	{
		//do
		//{
			time_t tt = time(NULL);
			char tTmp[32] = "\0";
			tm *t = localtime(&tt);
			sprintf(tTmp, "%02d:%02d:%02d ", t->tm_hour, t->tm_min, t->tm_sec);

			//gchar errMsg_s[256] = "\0";
			gchar g_out[1024 * 100 * 4] = "\0";
			int i_ret_r = 0;
			gchar errMsg_r[256] = "\0";
			char szRecieve[1024 * 100] = "";
			i_ret_r = pS->wait_2_read_line(atol(g_data_buf_len), szRecieve, errMsg_r, g_hex_output_checked);
			// printf("i_ret_r: %d\n", i_ret_r);
			if (i_ret_r != 0)
			{
				// usleep(50000);
				return TRUE;
			}

			if (strlen(errMsg_r) > 0)
			{
				strcat(g_out, errMsg_r);
				strcat(g_out, "\n");
			}
			else
			{
				if (g_hex_output_checked)
				{
					string sTmp = insert_space_split_2(szRecieve);
					strcat(g_out, tTmp);
					strcat(g_out, "<<");
					strcat(g_out, sTmp.c_str());
					strcat(g_out, "\n");
				}
				else
				{
					strcat(g_out, tTmp);
					strcat(g_out, "<<");
					strcat(g_out, szRecieve);
					strcat(g_out, "\n");
				}
			}

			GtkTextIter start, end;
			gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(data), &start, &end);
			gtk_text_buffer_insert(GTK_TEXT_BUFFER(data), &end, g_out, strlen(g_out));

		//	clk_end = clock();
		//} while ((clk_end - clk_start) / CLOCKS_PER_SEC * 1000 < u_timeout);
	}

	return TRUE;
}

int main(int argc, char *argv[])
{
	g_text2send = (gchar*)malloc(MAX_SEND*sizeof(gchar));
	if (NULL == g_text2send)
		return -1;
	
	g_crc_in = (gchar*)malloc(MAX_SEND*sizeof(gchar));

	pS = new my_serial_ctrl();

	GtkBuilder *builder = NULL;
	GObject *window = NULL;
	GObject *comboBoxText = NULL;
	GObject *button = NULL;
	GObject *chk_btn = NULL;
	GObject *textView = NULL;
	GObject *textBuffer = NULL;
	GObject *entry = NULL;
	GError *error = NULL;

	//const gchar *entry_port = "/dev/ttyUSB0";
	//const gchar *entry_baud = "57600";

	gtk_init(&argc, &argv);

	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, "/usr/local/bin/SerialPortToolGUI.ui", &error) == 0)
	{
		g_printerr("Error loading file: %s\n", error->message);
		g_clear_error(&error);
		if (gtk_builder_add_from_file(builder, "./SerialPortToolGUI.ui", &error) == 0)
		{
			g_printerr("Error loading file: %s\n", error->message);
			g_clear_error(&error);
			return 1;
		}
	}

	window = gtk_builder_get_object(builder, "mainWindow");
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	button = gtk_builder_get_object(builder, "btn_open");
	g_signal_connect(button, "clicked", G_CALLBACK(open_callback), (gpointer)window);

	button = gtk_builder_get_object(builder, "btn_close");
	g_signal_connect(button, "clicked", G_CALLBACK(close_callback), (gpointer)window);

	button = gtk_builder_get_object(builder, "btn_timeout_setting");
	g_signal_connect(button, "clicked", G_CALLBACK(btn_timeout_setting_callback), (gpointer)window);

	button = gtk_builder_get_object(builder, "btn_more_setting");
	g_signal_connect(button, "clicked", G_CALLBACK(btn_more_setting_callback), (gpointer)window);

	chk_btn = gtk_builder_get_object(builder, "chkbtn_hex_output");
	g_signal_connect(chk_btn, "released", G_CALLBACK(chk_btn_output_callback), NULL);

	chk_btn = gtk_builder_get_object(builder, "chkbtn_hex_send");
	g_signal_connect(chk_btn, "released", G_CALLBACK(chk_btn_send_callback), NULL);

	//textView = gtk_builder_get_object(builder, "home_tv_send");
	//gtk_widget_grab_focus((GtkWidget*)textView);
	//buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
	textBuffer = gtk_builder_get_object(builder, "textbuffer_send");
	g_signal_connect(textBuffer, "changed", G_CALLBACK(text_view_send_callback), NULL);
	button = gtk_builder_get_object(builder, "btn_clear_send");
	g_signal_connect(button, "clicked", G_CALLBACK(clear_callback), (gpointer)textBuffer);

	textView = gtk_builder_get_object(builder, "home_tv_output");
	textBuffer = gtk_builder_get_object(builder, "textbuffer_output");
	g_signal_connect(textBuffer, "changed", G_CALLBACK(scroll_output_callback), (gpointer)textView);
	button = gtk_builder_get_object(builder, "btn_send");
	g_signal_connect(button, "clicked", G_CALLBACK(text_view_output_callback), (gpointer)textBuffer);
	button = gtk_builder_get_object(builder, "btn_clear_output");
	g_signal_connect(button, "clicked", G_CALLBACK(clear_callback), (gpointer)textBuffer);

	gdk_threads_add_idle(readDaemon, (gpointer)textBuffer);

	//textView = gtk_builder_get_object(builder, "utility_tv_send");
	//gtk_widget_grab_focus((GtkWidget*)textView);
	textBuffer = gtk_builder_get_object(builder, "textBuffer_utility_crc_in");
	g_signal_connect(textBuffer, "changed", G_CALLBACK(text_view_crc_in_callback), NULL);
	button = gtk_builder_get_object(builder, "btn_calculate");
	textBuffer = gtk_builder_get_object(builder, "textBuffer_utility_crc_out");
	g_signal_connect(button, "clicked", G_CALLBACK(tv_crc_callback), (gpointer)textBuffer);

	comboBoxText = gtk_builder_get_object(builder, "cbt_crc_select");
	g_signal_connect(comboBoxText, "changed", G_CALLBACK(cbt_crc_select_callback), NULL);

	entry = gtk_builder_get_object(builder, "entry_read_buf_len");
	g_signal_connect(entry, "changed", G_CALLBACK(entry_callback), NULL);

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

		g_free(gc);
		gc = NULL;
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
		printf("free pS ... \n");
		delete pS;
		pS = NULL;
	}

	if (NULL != g_crc_in)
	{
		printf("free g_crc_in ... \n");
		free(g_crc_in);
		g_crc_in = NULL;
	}

	if (NULL != g_text2send)
	{
		printf("free g_text2send ... \n");
		free(g_text2send);
		g_text2send = NULL;
	}

	return 0;
}
