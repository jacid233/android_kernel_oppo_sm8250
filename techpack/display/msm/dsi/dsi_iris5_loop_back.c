#include <drm/drm_mipi_dsi.h>
#include <video/mipi_display.h>
#include <linux/of_gpio.h>
#include <dsi_drm.h>
#include <sde_encoder_phys.h>
#include "dsi_iris5_api.h"
#include "dsi_iris5_lightup.h"
#include "dsi_iris5_lightup_ocp.h"
#include "dsi_iris5_lp.h"
#include "dsi_iris5_pq.h"
#include "dsi_iris5_ioctl.h"
#include "dsi_iris5_lut.h"
#include "dsi_iris5_mode_switch.h"
#include "dsi_iris5_osd.h"
#include "iris_log.h"

/*
 * These files include source code for the Licensed Software that is provided under the terms and
 * conditions of a Master Software License Agreement and its applicable schedules.
 */

int iris_loop_back_reset(void)
{
	int rc = 0;
	struct iris_cfg *pcfg = iris_get_cfg_by_index(DSI_PRIMARY);
	struct dsi_panel *panel;

	panel = pcfg->panel;
	iris_one_wired_cmd_send(panel, RESET_SYS);
	return rc;
}

static u32 addrs[66] = {
0xf000004c, 0xf000004c, 0xf0000048, 0xf1680008, 0xf16e0008, 0xf1a20044,
0xf1a40044, 0xf158000c, 0xf1580290, 0xf1560118, 0xf1a00060, 0xf1520058,
0xf10c0000, 0xf1500404, 0xf12c0000, 0xf12d0000, 0xf1640054, 0xf1200020,
0xf120002c, 0xf120009c, 0xf1210000, 0xf1240004, 0xf1240008, 0xf124000c,
0xf1240018, 0xf124003c, 0xf1240074, 0xf1240150, 0xf1240170, 0xf1241004,
0xf1241084, 0xf1241098, 0xf124109c, 0xf12410b0, 0xf12410e8, 0xf1240000,
0xf1250000, 0xf1280008, 0xf1280038, 0xf12800c4, 0xf1281004, 0xf1281014,
0xf1281028, 0xf1290000, 0xf1220000, 0xf1220004, 0xf1220008, 0xf1220014,
0xf122001c, 0xf1220064, 0xf16400b8, 0xf1a40000, 0xf1a40008, 0xf1a40018,
0xf1a4001c, 0xf1a40024, 0xf1a40028, 0xf1a4002c, 0xf1500098, 0xf1500000,
0xf1580000, 0xf1580014, 0xf1580290, 0xf1400024, 0xf140002c, 0xf141ff00
};

static u32 values[66] = {
0x0c011800, 0x0e019c00, 0x000026a0, 0x00000800, 0x00000800, 0x00001FFF,
0x00001FFF, 0x00000800, 0x00000001, 0x00003FFF, 0x00010800, 0x00003FFF,
0x00001484, 0x00000800, 0x0000d04d, 0x00000000, 0x000013ff, 0x020002c3,
0x0000000a, 0x0000000c, 0x0000000f, 0x00401384, 0x30800065, 0x50208800,
0x04380438, 0x00000020, 0xffffffff, 0x00000545, 0x00000003, 0x00020888,
0xe4100010, 0x0000005a, 0x00040000, 0x11210100, 0x0000005a, 0xa0e8000c,
0x00000100, 0x00000001, 0x04380438, 0x00000003, 0x00020888, 0x0000005a,
0x00000001, 0x00000004, 0xe0008007, 0x21008801, 0x4780010e, 0x00044100,
0x20000186, 0x00000002, 0xb46c343c, 0x00037762, 0x00080000, 0x00020003,
0x00020003, 0x00000019, 0x09800438, 0x00080000, 0x00000000, 0xd0840421,
0x00010040, 0x00000010, 0x00000002, 0x00020000, 0x00200195, 0x00000101
};

void iris_ocp_i3c_write(u32 addr, u32 value)
{
	u32 values[2];

	values[0] = addr;
	values[1] = value;

	IRIS_LOGD("i3c write, addr = %x, value = %x", addr, value);
	iris_i2c_ocp_single_write(values, 1);
}

u32 iris_ocp_i3c_read(u32 addr, u32 mode)
{
	u32 values[2];
	u32 ret = 0;

	values[0] = addr;
	values[1] = mode;

	ret = iris_i2c_ocp_read(values, 1, 0);
	if (ret)
		pr_err("iris_ocp_i3c_read error!\n");

	return values[0];
}


u32 iris5_loop_back_verify(void)
{

	u32 i, r, g, b;
	struct iris_cfg *pcfg;
	u32 ret = 0;
	u32 standard_rgbsum[3] = {0x40d1a890, 0x318c343c, 0x37839da4};

	pcfg = iris_get_cfg();

	IRIS_LOGD("loop back verify!\n");

	iris_ocp_i3c_write(0xf0000044, 0x00000400);
	iris_ocp_i3c_write(0xf00000c0, 0x00000055);
	iris_ocp_i3c_write(0xf0000060, 0x0f0303fe);
	iris_ocp_i3c_write(0xf0000060, 0x0f0303f6);
	iris_ocp_i3c_write(0xf0000060, 0x0f0303fe);
	mdelay(100);

	//iris_ocp_i3c_write(0xf0000050, 0x00003f00);
	iris_ocp_i3c_write(0xf0000004, 0x002a80a9);
	iris_ocp_i3c_write(0xf0000008, 0x0010f018);
	iris_ocp_i3c_write(0xf0000000, 0x00000081);
	iris_ocp_i3c_write(0xf0000000, 0x00000083);
	iris_ocp_i3c_write(0xf0000000, 0x00000081);
	mdelay(10);
	iris_ocp_i3c_write(0xf0000050, 0x00000000);

	iris_ocp_i3c_write(0xf120005c, 0x00fffffe);

	for (i = 0; i < 66; i++)
		iris_ocp_i3c_write(addrs[i], values[i]);

	mdelay(1);
	iris_ocp_i3c_write(0xf1200020, 0x02000ac3);
	iris_ocp_i3c_write(0xf1210000, 0x3);
	mdelay(20);
	iris_ocp_i3c_write(0xf1200020, 0x020002c3);

	r = iris_ocp_i3c_read(0xf12401a8, DSI_CMD_SET_STATE_HS);
	g = iris_ocp_i3c_read(0xf12401ac, DSI_CMD_SET_STATE_HS);
	b = iris_ocp_i3c_read(0xf12401b0, DSI_CMD_SET_STATE_HS);
	IRIS_LOGD("r = 0x%08x, g = 0x%08x, b = 0x%08x\n", r, g ,b);

	if ((r == standard_rgbsum[0]) && (g == standard_rgbsum[1]) && (b == standard_rgbsum[2])) {
		ret = 0;
	} else {
		ret = 3;
	}

	return ret;

}

int iris_loop_back_validate(void)
{

	int rc = 0;
	int temp = 0;
	struct iris_cfg *pcfg = NULL;

	pcfg = iris_get_cfg();

	IRIS_LOGI("[%s:%d] loop back test.", __func__, __LINE__);

	rc = iris_loop_back_reset();
	if (rc) {
		IRIS_LOGW("[%s:%d] loop back iris reset rc = %d", __func__, __LINE__, rc);
		return rc;
	}

	mdelay(10);
	temp = iris_ocp_i3c_read(0xf00000d0, DSI_CMD_SET_STATE_HS);
	IRIS_LOGD("%s,%d: value = 0x%x", __func__, __LINE__, temp);

	mdelay(10);

	temp = iris_ocp_i3c_read(0xf1800000, DSI_CMD_SET_STATE_HS);
	IRIS_LOGD("%s,%d: value = 0x%x", __func__, __LINE__, temp);
	temp &= (~0x1);
	IRIS_LOGD("%s,%d: value = 0x%x", __func__, __LINE__, temp);
	iris_ocp_i3c_write(0xf1800000, temp);
	temp = iris_ocp_i3c_read(0xf1800000, DSI_CMD_SET_STATE_HS);
	IRIS_LOGD("%s,%d: value = 0x%x", __func__, __LINE__, temp);

	rc = iris5_loop_back_verify();
	if (rc) {
		IRIS_LOGE("[%s:%d] rc = %d", __func__, __LINE__, rc);
		return rc;
	}

	rc = iris_loop_back_reset();
	if (rc) {
		IRIS_LOGW("[%s:%d] loop back iris reset rc = %d", __func__, __LINE__, rc);
		return rc;
	}

	iris_abyp_lp(ABYP_POWER_DOWN_PLL); //CID90440

	return rc;
}
