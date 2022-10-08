set(SUB_DIRS
    "." "blufi" "wifi_manager"
)

idf_component_register(
    SRC_DIRS        ${SUB_DIRS}
    INCLUDE_DIRS    ${SUB_DIRS}
)
