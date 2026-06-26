需要移植

中间层控制程序 不需要修改
openbl_core .c/.h
openbl_mem .c/.h
openbl_usart_cmd .c/.h   需要哪个接口移植哪个

需要修改
common_interface .c/.h
flash_interface .c/.h
iwdg_interface .c/.h
optionbytes_interface .c/.h
ram_interface .c/.h 
usart_interface .c/.h 

openbootloader_conf.h
interfaces_conf.h
platform.h


app_openbootloader .c/.h

