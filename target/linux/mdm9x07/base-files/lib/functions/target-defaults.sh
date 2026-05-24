#!/bin/ash

. /lib/functions.sh
. /usr/share/libubox/jshn.sh

ucidef_add_nand_info() {
	local model="$1"

	model=${model:0:7}

	json_select_object nand

	case "$model" in
	TRB1412)
		json_add_int blocksize 128
		json_add_int pagesize 2048
		json_add_int subpagesize 2048
	;;
	TRB1422 |\
	TRB1423 |\
	TRB1424 |\
	TRB1452 |\
	TRB1414 |\
	TRB1434 |\
	TRB1454 |\
	TRB1404)
		json_add_int blocksize 256
		json_add_int pagesize 4096
		json_add_int subpagesize 4096
	;;
	TRB14*0)
		json_add_int blocksize 128
		json_add_int pagesize 2048
		json_add_int subpagesize 2048
	;;
	TRB14*1)
		json_add_int blocksize 256
		json_add_int pagesize 4096
		json_add_int subpagesize 4096
	;;
	esac

	json_select ..
}

ucidef_add_rmnet_proto_info() {
	local model="$1"

	case "$model" in
	TRB14*)
		json_add_string custom_proto "connm"
		json_add_string custom_ifname "rmnet0"
		;;
	esac
}

ucidef_add_custom_modem_info() {
	#Parameters: model primary
	local model vendor product boudrate gps type desc control region modem_counter cmport
	modem_counter=1
	cmport=7
	json_select_array "modems"

	model="$1"
	model=${model:0:7}

	case "$model" in
	TRB1422)
		vendor=05c6
		product=9215
		;;
	TRB1412 |\
	TRB1423 |\
	TRB1452 |\
	TRB140* |\
	TRB143*)
		vendor=2c7c
		product=0125
		cmport=9
		;;
	TRB14*)
		vendor=2c7c
		product=0121
		cmport=9
		;;
	esac

	case "$model" in
	TRB1412 |\
	TRB14*0)
		region="EU"
		;;
	TRB1422)
		region="CE"
		;;
	TRB1423 |\
	TRB1452 |\
	TRB14*1)
		region="AU"
		;;
	TRB1404 |\
	TRB1414 |\
	TRB1424 |\
	TRB1434 |\
	TRB1454)
		region="J"
		;;
	esac

	[ -f "/lib/network/wwan/$vendor:$product" ] && {
		devicename="3-1"
		json_set_namespace defaults old_cb
		json_load "$(cat /lib/network/wwan/$vendor:$product)"
		json_get_vars gps boudrate type desc control stop_bits
		json_set_namespace $old_cb

		[ "${devicename%%:*}" = "$devicename" ] && {
			json_add_object
				json_add_string id "$devicename"
				json_add_string num "$modem_counter"
				json_add_string vendor "$vendor"
				json_add_string product "$product"
				json_add_string stop_bits "$stop_bits"
				json_add_string gps "$gps"
				json_add_string boudrate "$boudrate"
				json_add_string type "$type"
				json_add_string desc "$desc"
				json_add_string region "$region"
				json_add_string control "$control"
				json_add_int simcount 1
				json_add_boolean builtin 1
				json_add_string cmd_port "/dev/smd$cmport"
				json_add_string data_port "/dev/smd8"
				json_add_string wdm_port "/dev/cdc-wdm0"
				json_add_boolean primary 1
				json_add_boolean multi_apn 0
			json_close_object
		}
	}

	json_select ..
}

ucidef_add_model_expand() {
	local model="$1"

	json_select_object model
	json_add_string pid "0x0002"
	json_add_string vid "0x1D12"
	json_select ..
}

boot_target_pre_board_detect() {
	local param="$(/sbin/mnf_info --name)"
	[ "${param:0:6}" = "TRB141" ] && rm /etc/modules.d/spi-dev
}

sim_has_esim_flag() {
	local num sim_cfg
	for num in $(seq 1 4); do
		sim_cfg="$(/sbin/mnf_info -C "$num" 2>/dev/null)"
		[ -z "$sim_cfg" ] && return 1
		[ "${sim_cfg:2:1}" -eq 2 ] && return 0
	done
	return 1
}

ucidef_target_defaults() {
	local model="$1"

	ucidef_add_nand_info "$model"
	ucidef_add_rmnet_proto_info "$model"
	ucidef_add_custom_modem_info "$model"
	ucidef_add_model_expand "$model"
	sim_has_esim_flag && ucidef_set_esim

	case "$model" in
	TRB140*)
		[ "${model:7:1}" = "2" ] && ucidef_unset_hwinfo ios

		[ "${model:6:1}" = "4" ] && \
			ucidef_set_release_version "7.4.2"

		ucidef_set_interface_default_macaddr "lan" "$(macaddr_canonicalize $(/sbin/mnf_info -m))"
	;;
	TRB141*)
		[ "${model:6:1}" = "4" ] && \
			ucidef_set_release_version "7.4.3"
	;;
	TRB142*)
		[ "${model:7:1}" = "2" ] && ucidef_unset_hwinfo ios

		if [ "${model:6:1}" = "3" ]; then
			ucidef_set_release_version "7.1"
		elif [ "${model:6:1}" = "4" ]; then
			ucidef_set_release_version "7.4.3"
		fi
	;;
	TRB143*)
		[ "${model:7:1}" = "2" ] && ucidef_unset_hwinfo ios

		if [ "${model:6:1}" = "4" ]; then
			ucidef_set_release_version "7.4.3"
		else
			ucidef_set_release_version "7.1.7"
		fi

		ucidef_set_interface_default_macaddr "lan" "$(macaddr_canonicalize $(/sbin/mnf_info -m))"
	;;
	TRB145*)
		[ "${model:7:1}" = "2" ] &&  ucidef_unset_hwinfo ios

		if [ "${model:6:1}" = "2" ]; then
			ucidef_set_release_version "7.1"
		elif [ "${model:6:1}" = "4" ]; then
			ucidef_set_release_version "7.4.3"
		fi
	;;
	esac
}
