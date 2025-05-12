# stm32 USB键盘

#### 介绍
本项目基于野火stm32指南者开发版，只是stm32F103芯片实现键盘的按键输入以及多媒体控制功能

程序的演示视频https://www.bilibili.com/video/BV1iV4y1H7dp
		       https://www.bilibili.com/video/BV1Cd4y197N1
文档包含了源程序，2个介绍USB协议的文档，需要查键值时可在文档对应的地方查找，还有一个测试开发板的原理图。
如果有用的话，记得给一个星星呀。
	
#### 程序结构说明
此程序在只调用了USB外设的PA11和PA12引脚和几个按键输入引脚(bsp_key.c)，对于STM32F103，只需要修改bsp_key中的引脚定义和gpio.c中的引脚初始化即可，LED可以不用。若用CubeMX重新覆盖此程序，则会将描述符覆盖掉，需要重新添加描述符，步骤如下。若想避免的话覆盖自己的HID，在USB外设中，选自定义HID设备即可。
	
	
#### 如何移植程序
使用CubeMX生成，添加USB库，初始化代码后，需要做如下操作
1. 修改HID报告描述符：在usbd_desc.c文件中找到数组 HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE]即为
HID报告描述的存放位置，初始化程序的描述符仅实现了鼠标的功能，可以删掉，用此程序描述符替换。HID报告描述符
事先定义了我们要给电脑发送的数据的格式，好让电脑识别，也可以根据需要自己修改描述符。
2. 替换完成后，需要修改描述符数组的大小，在usbd_hid.h中找到HID_MOUSE_REPORT_DESC_SIZE定义的地方，将其
修改为我们自己的报告描述符的大小，此程序的描述的大小为176字节。同时要修改发送一次数据字节的长度，在usbd_hid.h中
的HID_EPIN_SIZE，具体大小根据报告描述符定义的格式计算出，这个不包含ReportID所占用的一个字节。本例中
为8(键盘)+1(多媒体)+4(鼠标)=13字节。
3. usbd_hid.c中函数USBD_HID_CfgFSDesc()中，需要将nInterfaceProtocol项目改位0x01，这样插入电脑后，识别到的即为
键盘，不改的话识别为鼠标。usbd_conf.h中的HID_FS_BINTERVAL可设置设置电脑轮询的时间间隔，单位为ms，此程序设置为1，
两个发送函数之间的时间间隔尽量大于此时间
	
	
