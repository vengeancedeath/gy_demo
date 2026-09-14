
# idf_component_register(SRCS "LED.c"
#                     INCLUDE_DIRS "include"
#                     PRIV_REQUIRES esp_driver_gpio)

#set(src_dirs   ".")  有问题
# set(srcs "LED.c")
# set(include_dirs "include")
# set(priv_requires esp_driver_gpio)
# idf_component_register(SRCS "${srcs}"
#                     INCLUDE_DIRS "${include_dirs}"
#                     PRIV_REQUIRES "${priv_requires}")


set(src_dirs "src")
set(include_dirs "include")
set(priv_requires esp_driver_gpio)
idf_component_register(SRC_DIRS "${src_dirs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES "${priv_requires}")
