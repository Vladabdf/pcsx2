/* SPU2-X, A plugin for Emulating the Sound Processing Unit of the Playstation 2
 * Developed and maintained by the Pcsx2 Development Team.
 *
 * Original portions from SPU2ghz are (c) 2008 by David Quintana [gigaherz]
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 *
 */

#include "Global.h"
#include "Dialogs.h"
#include "Config.h"
#include "soundtouch/SoundTouch.h"

namespace SoundtouchCfg
{
// Timestretch Slider Bounds, Min/Max
static const int SequenceLen_Min = 20;
static const int SequenceLen_Max = 100;

static const int SeekWindow_Min = 10;
static const int SeekWindow_Max = 30;

static const int Overlap_Min = 5;
static const int Overlap_Max = 15;

static int SequenceLenMS = 30;
static int SeekWindowMS = 20;
static int OverlapMS = 10;

static void ClampValues()
{
    Clampify(SequenceLenMS, SequenceLen_Min, SequenceLen_Max);
    Clampify(SeekWindowMS, SeekWindow_Min, SeekWindow_Max);
    Clampify(OverlapMS, Overlap_Min, Overlap_Max);
}

void ApplySettings(soundtouch::SoundTouch &sndtouch)
{
    sndtouch.setSetting(SETTING_SEQUENCE_MS, SequenceLenMS);
    sndtouch.setSetting(SETTING_SEEKWINDOW_MS, SeekWindowMS);
    sndtouch.setSetting(SETTING_OVERLAP_MS, OverlapMS);
}

void ReadSettings()
{
    SequenceLenMS = CfgReadInt(L"SOUNDTOUCH", L"SequenceLengthMS", 30);
    SeekWindowMS = CfgReadInt(L"SOUNDTOUCH", L"SeekWindowMS", 20);
    OverlapMS = CfgReadInt(L"SOUNDTOUCH", L"OverlapMS", 10);

    ClampValues();
    WriteSettings();
}

void WriteSettings()
{
    CfgWriteInt(L"SOUNDTOUCH", L"SequenceLengthMS", SequenceLenMS);
    CfgWriteInt(L"SOUNDTOUCH", L"SeekWindowMS", SeekWindowMS);
    CfgWriteInt(L"SOUNDTOUCH", L"OverlapMS", OverlapMS);
}

#ifdef __unix__
static GtkWidget *seq_label, *seek_label, *over_label;
static GtkWidget *seq_slide, *seek_slide, *over_slide;


void restore_defaults()
{
    gtk_range_set_value(GTK_RANGE(seq_slide), 30);
    gtk_range_set_value(GTK_RANGE(seek_slide), 20);
    gtk_range_set_value(GTK_RANGE(over_slide), 10);
}


void DisplayDialog()
{
    int return_value;
    GtkWidget *dialog, *main_label, *adv_box;
    GtkWidget *default_button;

    ReadSettings();

    /* Create the widgets */
    dialog = gtk_dialog_new_with_buttons(
        "Advanced Settings",
        NULL, /* parent window*/
        (GtkDialogFlags)(GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
        "Cancel", GTK_RESPONSE_REJECT,
        "OK", GTK_RESPONSE_ACCEPT,
        NULL);

    main_label = gtk_label_new("These are advanced configuration options for fine tuning time stretching behavior. \nLarger values are better for slowdown, while smaller values are better for speedup (more then 60 fps.). \nAll options are in microseconds.");
    gtk_label_set_line_wrap(GTK_LABEL(main_label), true);

    default_button = gtk_button_new_with_label("Reset to Defaults");

    seq_label = gtk_label_new("Sequence Length");
    seq_slide = ps_gtk_hscale_new_with_range(SequenceLen_Min, SequenceLen_Max, 2);
    gtk_range_set_value(GTK_RANGE(seq_slide), SequenceLenMS);

    seek_label = gtk_label_new("Seek Window Size");
    seek_slide = ps_gtk_hscale_new_with_range(SeekWindow_Min, SeekWindow_Max, 2);
    gtk_range_set_value(GTK_RANGE(seek_slide), SeekWindowMS);

    over_label = gtk_label_new("Overlap");
    over_slide = ps_gtk_hscale_new_with_range(Overlap_Min, Overlap_Max, 2);
    gtk_range_set_value(GTK_RANGE(over_slide), OverlapMS);

    adv_box = ps_gtk_vbox_new(5);

    gtk_box_pack_start(GTK_BOX(adv_box), main_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(adv_box), default_button, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(adv_box), seq_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(adv_box), seq_slide, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(adv_box), seek_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(adv_box), seek_slide, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(adv_box), over_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(adv_box), over_slide, TRUE, TRUE, 5);

    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), adv_box);
    g_signal_connect_swapped(default_button, "clicked", G_CALLBACK(restore_defaults), default_button);
    gtk_widget_show_all(dialog);

    return_value = gtk_dialog_run(GTK_DIALOG(dialog));

    if (return_value == GTK_RESPONSE_ACCEPT) {
        SequenceLenMS = gtk_range_get_value(GTK_RANGE(seq_slide));
        SeekWindowMS = gtk_range_get_value(GTK_RANGE(seek_slide));
        OverlapMS = gtk_range_get_value(GTK_RANGE(over_slide));
    }

    gtk_widget_destroy(dialog);

    WriteSettings();
}
#else
void DisplayDialog()
{
}

void restore_defaults()
{
}
#endif
} // namespace SoundtouchCfg
