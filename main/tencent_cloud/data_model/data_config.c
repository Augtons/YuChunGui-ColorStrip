/*-----------------data config start  -------------------*/ 

#define MAX_ARRAY_JSON_STR_LEN (512)
#define MAX_SAMPLE_ARRAY_SIZE (16)

#define TOTAL_PROPERTY_COUNT 8

static sDataPoint    sg_DataTemplate[TOTAL_PROPERTY_COUNT];

ProductDataDefine     sg_ProductData;

static void _init_data_template(void)
{
    sg_ProductData.m_power_switch = 0;
    sg_DataTemplate[0].data_property.data = &sg_ProductData.m_power_switch;
    sg_DataTemplate[0].data_property.key  = "power_switch";
    sg_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_DataTemplate[0].state = eCHANGED;

    sg_ProductData.m_brightness = 100;
    sg_DataTemplate[1].data_property.data = &sg_ProductData.m_brightness;
    sg_DataTemplate[1].data_property.key  = "brightness";
    sg_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[1].state = eCHANGED;

    sg_ProductData.m_humi = 0;
    sg_DataTemplate[2].data_property.data = &sg_ProductData.m_humi;
    sg_DataTemplate[2].data_property.key  = "humi";
    sg_DataTemplate[2].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[2].state = eCHANGED;

    sg_ProductData.m_color_mode = 2;
    sg_DataTemplate[3].data_property.data = &sg_ProductData.m_color_mode;
    sg_DataTemplate[3].data_property.key  = "color_mode";
    sg_DataTemplate[3].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_DataTemplate[3].state = eCHANGED;

    sg_ProductData.m_color_red = 0;
    sg_DataTemplate[4].data_property.data = &sg_ProductData.m_color_red;
    sg_DataTemplate[4].data_property.key  = "color_red";
    sg_DataTemplate[4].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[4].state = eCHANGED;

    sg_ProductData.m_color_blue = 0;
    sg_DataTemplate[5].data_property.data = &sg_ProductData.m_color_blue;
    sg_DataTemplate[5].data_property.key  = "color_blue";
    sg_DataTemplate[5].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[5].state = eCHANGED;

    sg_ProductData.m_color_green = 255;
    sg_DataTemplate[6].data_property.data = &sg_ProductData.m_color_green;
    sg_DataTemplate[6].data_property.key  = "color_green";
    sg_DataTemplate[6].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[6].state = eCHANGED;

    sg_ProductData.m_temp = 0;
    sg_DataTemplate[7].data_property.data = &sg_ProductData.m_temp;
    sg_DataTemplate[7].data_property.key  = "temp";
    sg_DataTemplate[7].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_DataTemplate[7].state = eCHANGED;

};
