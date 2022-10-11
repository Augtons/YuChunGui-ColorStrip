/*-----------------data config start  -------------------*/ 

#define MAX_ARRAY_JSON_STR_LEN (512)
#define MAX_SAMPLE_ARRAY_SIZE (16)

#define TOTAL_PROPERTY_COUNT 5

static sDataPoint    sg_DataTemplate[TOTAL_PROPERTY_COUNT];

typedef struct _ProductDataDefine {
    TYPE_DEF_TEMPLATE_BOOL m_power_switch;
    TYPE_DEF_TEMPLATE_INT m_brightness;
    TYPE_DEF_TEMPLATE_STRING m_color_value[2048+1];
    TYPE_DEF_TEMPLATE_FLOAT m_temp;
    TYPE_DEF_TEMPLATE_INT m_humi;
} ProductDataDefine;

static   ProductDataDefine     sg_ProductData;

static void _init_data_template(void)
{
    sg_ProductData.m_power_switch = 0;
    sg_DataTemplate[0].data_property.data = &sg_ProductData.m_power_switch;
    sg_DataTemplate[0].data_property.key  = "power_switch";
    sg_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_DataTemplate[0].state = eCHANGED;

    sg_ProductData.m_brightness = 0;
    sg_DataTemplate[1].data_property.data = &sg_ProductData.m_brightness;
    sg_DataTemplate[1].data_property.key  = "brightness";
    sg_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[1].state = eCHANGED;

    sg_ProductData.m_color_value[0] = '\0';
    sg_DataTemplate[2].data_property.data = sg_ProductData.m_color_value;
    sg_DataTemplate[2].data_property.data_buff_len = sizeof(sg_ProductData.m_color_value)/sizeof(sg_ProductData.m_color_value[0]);
    sg_DataTemplate[2].data_property.key  = "color_value";
    sg_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DataTemplate[2].state = eCHANGED;

    sg_ProductData.m_temp = 0;
    sg_DataTemplate[3].data_property.data = &sg_ProductData.m_temp;
    sg_DataTemplate[3].data_property.key  = "temp";
    sg_DataTemplate[3].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_DataTemplate[3].state = eCHANGED;

    sg_ProductData.m_humi = 0;
    sg_DataTemplate[4].data_property.data = &sg_ProductData.m_humi;
    sg_DataTemplate[4].data_property.key  = "humi";
    sg_DataTemplate[4].data_property.type = TYPE_TEMPLATE_INT;
    sg_DataTemplate[4].state = eCHANGED;

};

