/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <string.h>
#include <err.h>
#include <smem.h>
#include <msm_panel.h>
#include <board.h>
#include <mipi_dsi.h>
#include <target/display.h>

#include "include/panel.h"
#include "panel_display.h"

/*---------------------------------------------------------------------------*/
/* GCDB Panel Database                                                       */
/*---------------------------------------------------------------------------*/
#include "include/panel_truly_1080p_video.h"
#include "include/panel_truly_1080p_cmd.h"
#include "include/panel_otm1906c_1080p_cmd.h"

/*---------------------------------------------------------------------------*/
/* static panel selection variable                                           */
/*---------------------------------------------------------------------------*/
enum {
	TRULY_1080P_VIDEO_PANEL,
	TRULY_1080P_CMD_PANEL,
	OTM1906C_1080P_CMD_PANEL,
	UNKNOWN_PANEL
};

uint32_t panel_regulator_settings[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * The list of panels that are supported on this target.
 * Any panel in this list can be selected using fastboot oem command.
 */
static struct panel_list supp_panels[] = {
	{"truly_1080p_video", TRULY_1080P_VIDEO_PANEL},
	{"truly_1080p_cmd", TRULY_1080P_CMD_PANEL},
};

static uint32_t panel_id;

int oem_panel_rotation()
{
	return NO_ERROR;
}

int oem_panel_on()
{
	/*
	 *OEM can keep their panel specific on instructions in this
	 *function
	 */
	return NO_ERROR;
}

int oem_panel_off()
{
	/* OEM can keep their panel specific off instructions
	 * in this function
	 */
	return NO_ERROR;
}

static int init_panel_data(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	int pan_type = PANEL_TYPE_DSI;

	switch (panel_id) {
	case TRULY_1080P_VIDEO_PANEL:
		panelstruct->paneldata    = &truly_1080p_video_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &truly_1080p_video_panel_res;
		panelstruct->color        = &truly_1080p_video_color;
		panelstruct->videopanel   = &truly_1080p_video_video_panel;
		panelstruct->commandpanel = &truly_1080p_video_command_panel;
		panelstruct->state        = &truly_1080p_video_state;
		panelstruct->laneconfig   = &truly_1080p_video_lane_config;
		panelstruct->paneltiminginfo
			= &truly_1080p_video_timing_info;
		panelstruct->panelresetseq
					 = &truly_1080p_video_panel_reset_seq;
		panelstruct->backlightinfo = &truly_1080p_video_backlight;
		pinfo->mipi.panel_on_cmds
			= truly_1080p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_1080P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_1080p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_1080P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			truly_1080p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_1080P_VIDEO_SIGNATURE;
		break;
	case TRULY_1080P_CMD_PANEL:
		panelstruct->paneldata    = &truly_1080p_cmd_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &truly_1080p_cmd_panel_res;
		panelstruct->color        = &truly_1080p_cmd_color;
		panelstruct->videopanel   = &truly_1080p_cmd_video_panel;
		panelstruct->commandpanel = &truly_1080p_cmd_command_panel;
		panelstruct->state        = &truly_1080p_cmd_state;
		panelstruct->laneconfig   = &truly_1080p_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &truly_1080p_cmd_timing_info;
		panelstruct->panelresetseq
					 = &truly_1080p_cmd_panel_reset_seq;
		panelstruct->backlightinfo = &truly_1080p_cmd_backlight;
		pinfo->mipi.panel_on_cmds
			= truly_1080p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_1080P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_1080p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_1080P_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			truly_1080p_cmd_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_1080P_CMD_SIGNATURE;
		break;
	case OTM1906C_1080P_CMD_PANEL:
		panelstruct->paneldata    = &otm1906c_1080p_cmd_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &otm1906c_1080p_cmd_panel_res;
		panelstruct->color        = &otm1906c_1080p_cmd_color;
		panelstruct->videopanel   = &otm1906c_1080p_cmd_video_panel;
		panelstruct->commandpanel = &otm1906c_1080p_cmd_command_panel;
		panelstruct->state        = &otm1906c_1080p_cmd_state;
		panelstruct->laneconfig   = &otm1906c_1080p_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &otm1906c_1080p_cmd_timing_info;
		panelstruct->panelresetseq
					 = &otm1906c_1080p_cmd_panel_reset_seq;
		panelstruct->backlightinfo = &otm1906c_1080p_cmd_backlight;
		pinfo->mipi.panel_on_cmds
			= otm1906c_1080p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= OTM1906C_1080P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= otm1906c_1080p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= OTM1906C_1080P_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			otm1906c_1080p_cmd_timings, TIMING_SIZE);
		pinfo->mipi.signature = OTM1906C_1080P_CMD_SIGNATURE;
		break;
	case UNKNOWN_PANEL:
	default:
		memset(panelstruct, 0, sizeof(struct panel_struct));
		memset(pinfo->mipi.panel_on_cmds, 0,
					sizeof(struct mipi_dsi_cmd));
		pinfo->mipi.num_of_panel_on_cmds = 0;
		memset(pinfo->mipi.panel_off_cmds, 0,
					sizeof(struct mipi_dsi_cmd));
		pinfo->mipi.num_of_panel_off_cmds = 0;
		memset(phy_db->timing, 0, TIMING_SIZE);
		pan_type = PANEL_TYPE_UNKNOWN;
		break;
	}
	return pan_type;
}

int oem_panel_select(const char *panel_name, struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	uint32_t hw_id = board_hardware_id();
	int32_t panel_override_id;

	if (panel_name) {
		panel_override_id = panel_name_to_id(supp_panels,
				ARRAY_SIZE(supp_panels), panel_name);

		if (panel_override_id < 0) {
			dprintf(CRITICAL, "Not able to search the panel:%s\n",
					 panel_name + strspn(panel_name, " "));
		} else if (panel_override_id < UNKNOWN_PANEL) {
			/* panel override using fastboot oem command */
			panel_id = panel_override_id;

			dprintf(INFO, "OEM panel override:%s\n",
					panel_name + strspn(panel_name, " "));
			goto panel_init;
		}
	}

	switch (hw_id) {
	case HW_PLATFORM_MTP:
		panel_id = TRULY_1080P_VIDEO_PANEL;
		break;
	case HW_PLATFORM_SURF:
		panel_id = TRULY_1080P_VIDEO_PANEL;
		break;
	case HW_PLATFORM_QRD:
		panel_id = OTM1906C_1080P_CMD_PANEL;
		break;
	default:
		dprintf(CRITICAL, "Display not enabled for %d HW type\n",
			hw_id);
		return PANEL_TYPE_UNKNOWN;
	}

panel_init:
	/*
	 * Update all data structures after 'panel_init' label. Only panel
	 * selection is supposed to happen before that.
	 */
	memcpy(panel_regulator_settings,
			dcdc_regulator_settings, REGULATOR_SIZE);
	pinfo->pipe_type = MDSS_MDP_PIPE_TYPE_RGB;
	return init_panel_data(panelstruct, pinfo, phy_db);
}
