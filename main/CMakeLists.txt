set(SINGLE_DIRS
    "." "system" "system/blufi" "system/wifi"
)

# Tencent Cloud SDK
set(TENCENT_CLOUD_INCLUDE
    "tencent_cloud"
    "tencent_cloud/include"
    "tencent_cloud/include/exports"
    "tencent_cloud/sdk_src/internal_inc"
    "tencent_cloud/data_model"
)

set(TENCENT_CLOUD_SRCS
    "."
    "tencent_cloud"
    "tencent_cloud/platform"
    "tencent_cloud/sdk_src"
)


idf_component_register(
    SRC_DIRS
        ${SINGLE_DIRS} ${TENCENT_CLOUD_SRCS}
    INCLUDE_DIRS
        ${SINGLE_DIRS} ${TENCENT_CLOUD_INCLUDE}
)
