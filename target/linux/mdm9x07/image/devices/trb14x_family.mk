include $(INCLUDE_DIR)/hardware.mk

define Device/teltonika_trb14_common
	HARDWARE/System_Characteristics/CPU := ARM Cortex-A7 1.2 GHz
	HARDWARE/System_Characteristics/RAM := $(HW_RAM_SIZE_128M), $(HW_RAM_TYPE_DDR2)
	HARDWARE/System_Characteristics/Flash_Storage := $(HW_FLASH_SIZE_512M) $(HW_FLASH_TYPE_SPI)
	TECHNICAL/Power/Connector := $(HW_POWER_CONNECTOR_4PIN)
	TECHNICAL/Power/Power_Consumption := < 5 W
	TECHNICAL/Power/Input_Voltage_Range := $(HW_POWER_VOLTAGE_4PIN_30V)
	TECHNICAL/Physical_Interfaces/Power := $(HW_INTERFACE_POWER_4PIN)
	TECHNICAL/Physical_Interfaces/Antennas := 1 x SMA for LTE
	TECHNICAL/Physical_Interfaces/SIM := 1 x SIM slot (Mini SIM - 2FF), 1.8 V/3 V
	TECHNICAL/Physical_Interfaces/Reset := $(HW_INTERFACE_RESET_BTN)
	TECHNICAL/Physical_Interfaces/USB := 1 x Virtual network interface via micro USB
	TECHNICAL/Physical_Specification/Casing_Material := $(HW_PHYSICAL_HOUSING_AL)
	TECHNICAL/Physical_Specification/Mounting_Options := $(HW_PHYSICAL_MOUNTING_KIT)
	TECHNICAL/Physical_Specification/Dimensions := 74.5 x 25 x 64.4 mm
	TECHNICAL/Operating_Environment/Operating_Temperature := $(HW_OPERATING_TEMP)
	TECHNICAL/Operating_Environment/Operating_Humidity := $(HW_OPERATING_HUMIDITY)
	TECHNICAL/Operating_Environment/Ingress_Protection_Rating := $(HW_OPERATING_PROTECTION_IP30)
endef

define Device/template_trb14_common
	$(Device/teltonika_trb1)

	DEVICE_INTERFACE_CONF := \
		lan default_ip 192.168.2.1

	DEVICE_NET_CONF :=       \
		vlans          4094, \
		max_mtu       1500

	DEVICE_FEATURES := gateway reset_button no-wired-wan 128mb_ram modbus dot1x-server single_port
endef

define Device/TEMPLATE_teltonika_trb140
	$(Device/teltonika_trb14_common)
	$(Device/template_trb14_common)
	DEVICE_MODEL := TRB140
	DEVICE_LAN_OPTION := "eth0 rndis0"
	DEVICE_FEATURES += mobile ethernet ios micro_usb port_link gigabit_port

	DEVICE_DOT1X_SERVER_CAPABILITIES := false false single_port


	HARDWARE/Mobile/Module := 4G LTE Cat 4 up to 150 DL/50 UL Mbps; 3G up to 21 DL/5.76 UL Mbps; 2G up to 236.8 DL/236.8 UL Kbps
	HARDWARE/Mobile/3GPP_Release := Release 11
	HARDWARE/Ethernet/Port :=1 $(HW_ETH_ETH_PORT)
	HARDWARE/Ethernet/Speed := $(HW_ETH_SPEED_1000)
	HARDWARE/Ethernet/Standard := $(HW_ETH_LAN_2_STANDARD)
	TECHNICAL/Power/PoE_Standards := $(HW_POWER_POE_PASSIVE_30V)
	TECHNICAL/Physical_Interfaces/Ethernet := 1 $(HW_ETH_RJ45_PORT), $(HW_ETH_SPEED_1000)
	TECHNICAL/Physical_Interfaces/Status_Leds := 3 x connection type status LEDs, 5 x connection strength LEDs, 2 x LAN status LEDs, 1x Power LED
	TECHNICAL/Input_Output/Input := 1 $(HW_INPUT_DI_30V)
	TECHNICAL/Input_Output/Output := 1 $(HW_OUTPUT_DO_30V)
	TECHNICAL/Physical_Interfaces/IO := $(HW_INTERFACE_IO_4PIN_IN_OUT)
	TECHNICAL/Physical_Specification/Weight := 134 g

endef
TARGET_DEVICES += TEMPLATE_teltonika_trb140

define Device/TEMPLATE_teltonika_trb141
	$(Device/teltonika_trb14_common)
	$(Device/template_trb14_common)
	DEVICE_MODEL := TRB141
	DEVICE_FEATURES += mobile ios micro_usb

	DEVICE_LAN_OPTION := "rndis0"

	HARDWARE/Mobile/Module := 4G LTE Cat 1 up to 10 DL/5 UL Mbps; 3G up to 384 DL/384 UL Kbps; 2G up to 296 DL/236.8 UL Kbps
	HARDWARE/Mobile/3GPP_Release := Release 12
	TECHNICAL/Physical_Interfaces/1_Wire:= 1 x 1-Wire interface on 16 pin connector
	TECHNICAL/Physical_Interfaces/Status_Leds := 3 x connection type status LEDs, 5 x connection strength LEDs, 1 x Power LED
	TECHNICAL/Input_Output/Input := 2 x Digital inputs (configurable passive or active), 1 x Isolated input, 1 x Analog input ( with 4-20 mA capability). 1 x Configurable Inputs. Digital input 0 - 5 V detected as logic low, 8 - 30 V detected as logic high.
	TECHNICAL/Input_Output/Output := 2 x Relay outputs (latching and non latching). 1 x Configurable Output, Open collector output, max output 30 V, 300 mA
	TECHNICAL/Physical_Interfaces/IO := 3 x Digital Inputs, 1 x Analog input, 2 x Relays on 16 pin connector, 2 x I/O pins on 4 pin power connector
	TECHNICAL/Physical_Specification/Weight := 136 g

endef
TARGET_DEVICES += TEMPLATE_teltonika_trb141

define Device/TEMPLATE_teltonika_trb142
	$(Device/teltonika_trb14_common)
	$(Device/template_trb14_common)
	DEVICE_MODEL := TRB142
	DEVICE_LAN_OPTION := "rndis0"
	DEVICE_FEATURES += mobile ios rs232 rs232_control micro_usb urc_control

	DEVICE_SERIAL_CAPABILITIES := \
		"rs232"                                                     \
			"300 600 1200 2400 4800 9600 19200 38400 57600 115200"  \
			"5 6 7 8"                                               \
			"rts/cts xon/xoff none"                                 \
			"1 2"                                                   \
			"even odd mark space none"                              \
			"none"                                                  \
			"/tty/ttyHS0"

	HARDWARE/Mobile/Module := 4G LTE Cat 1 up to 10 DL/5 UL Mbps; 3G up to 384 DL/384 UL Kbps; 2G up to 296 DL/236.8 UL Kbps
	HARDWARE/Mobile/3GPP_Release := Release 12
	TECHNICAL/Input_Output/Configurable_IO := 2 x Configurable Inputs/Outputs. Digital input 0 - 5 V detected as logic low, 8 - 30 V detected as logic high. Open collector \
											Configurable I/O output, max output 30 V, 300 mA. Configurable Inputs/Outputs are not available in TRB142*2****
	TECHNICAL/Physical_Interfaces/RS232 := 1 $(HW_INTERFACE_RS232_DB9)
	TECHNICAL/Physical_Interfaces/Status_Leds := 3 x connection type status LEDs, 5 x connection strength LEDs, 1 x Power LED
	TECHNICAL/Physical_Interfaces/IO := 2 x Configurable I/O pins on 4 pin power connector (I/O not available in TRB142*2****)
	TECHNICAL/Physical_Specification/Weight := 135 g

endef
TARGET_DEVICES += TEMPLATE_teltonika_trb142

define Device/TEMPLATE_teltonika_trb143
	$(Device/teltonika_trb14_common)
	$(Device/template_trb14_common)
	DEVICE_MODEL := TRB143
	DEVICE_LAN_OPTION := "eth0 rndis0"
	DEVICE_DOT1X_SERVER_CAPABILITIES := false false single_port
	DEVICE_FEATURES += mobile ethernet ios micro_usb port_link gigabit_port mbus

	DEVICE_SERIAL_CAPABILITIES := \
		"mbus"                             \
			"300 600 1200 2400 4800 9600"  \
			"8"                            \
			"none"                         \
			"1 2"                          \
			"even odd none"                \
			"half"                         \
			"/tty/ttyHS0"

	HARDWARE/Mobile/Module := 4G LTE Cat 4 up to 150 DL/50 UL Mbps; 3G up to 21 DL/5.76 UL Mbps; 2G up to 236.8 DL/236.8 UL Kbps
	HARDWARE/Mobile/3GPP_Release := Release 11
	HARDWARE/Ethernet/Port :=1 $(HW_ETH_ETH_PORT)
	HARDWARE/Ethernet/Speed := $(HW_ETH_SPEED_1000)
	HARDWARE/Ethernet/Standard := $(HW_ETH_LAN_2_STANDARD)
	TECHNICAL/Power/PoE_Standards := Possibility to power up through LAN1 port, not compatible with IEEE802.3af, 802.3at and 802.3bt standards, Mode B, 12 - 30 VDC
	TECHNICAL/Power/Power_Consumption := Idle 3 W, Max 6 W
	TECHNICAL/Power/Input_Voltage_Range := 12 – 30 VDC
	TECHNICAL/Input_Output/Input := 1 $(HW_INPUT_DI_30V)
	TECHNICAL/Input_Output/Output := 1 $(HW_OUTPUT_DO_30V)
	TECHNICAL/Physical_Interfaces/IO := $(HW_INTERFACE_IO_4PIN_IN_OUT)
	TECHNICAL/Physical_Interfaces/Status_Leds := 3 x connection type status LEDs, 3 x connection strength LEDs, 2 x ETH status LEDs, 1 x Power LED
	TECHNICAL/Physical_Interfaces/Ethernet := 1 $(HW_ETH_RJ45_PORT), $(HW_ETH_SPEED_1000)
	TECHNICAL/Physical_Interfaces/M_Bus := 1 x 2-pin M-Bus connection terminal
	TECHNICAL/Physical_Interfaces/USB :=
	TECHNICAL/Physical_Specification/Weight := 145 g
	TECHNICAL/Physical_Specification/Dimensions := 74.5 x 25 x 73 mm

endef
TARGET_DEVICES += TEMPLATE_teltonika_trb143

define Device/TEMPLATE_teltonika_trb145
	$(Device/teltonika_trb14_common)
	$(Device/template_trb14_common)
	DEVICE_MODEL := TRB145
	DEVICE_LAN_OPTION := "rndis0"
	DEVICE_FEATURES += mobile ios rs485 micro_usb urc_control

	DEVICE_SERIAL_CAPABILITIES := \
		"rs485"                                                    \
			"300 600 1200 2400 4800 9600 19200 38400 57600 115200  \
			 230400 460800 921600 1000000 3000000"                 \
			"5 6 7 8"                                              \
			"xon/xoff none"                                        \
			"1 2"                                                  \
			"even odd mark space none"                             \
			"half full"                                            \
			"/tty/ttyHS0"

	HARDWARE/Mobile/Module := 4G LTE Cat 1 up to 10 DL/5 UL Mbps; 3G up to 384 DL/384 UL Kbps; 2G up to 296 DL/236.8 UL Kbps
	HARDWARE/Mobile/3GPP_Release := Release 12
	TECHNICAL/Input_Output/Configurable_IO := 2 x Configurable Inputs/Outputs. Digital input 0 - 5 V detected as logic low, 8 - 30 V detected as logic high. Open collector output, max output 30 V, 300 mA (Configurable Inputs/Outputs are not available in TRB145*2****)
	TECHNICAL/Physical_Interfaces/IO := 2 x Configurable I/O pins on 4 pin power connector (I/O not available in TRB145*2****)
	TECHNICAL/Physical_Interfaces/Status_Leds := 3 x connection type status LEDs, 5 x connection strength LEDs, 1 x Power LED
	TECHNICAL/Physical_Interfaces/RS485 := 1 x 6-pin terminal block for 2-wire or 4-wire interface
	TECHNICAL/Physical_Specification/Weight := 130 g

endef
TARGET_DEVICES += TEMPLATE_teltonika_trb145
