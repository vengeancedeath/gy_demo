set(src_dirs 
            "src")
set(include_dirs 
            "include")
set(priv_requires 
            esp_driver_rmt 
            esp_timer)
idf_component_register(SRC_DIRS "${src_dirs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES "${priv_requires}"
                    )
