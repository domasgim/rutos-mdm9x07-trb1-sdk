. /lib/upgrade/common.sh

REQUIRE_IMAGE_METADATA=1
EMBARGO_PARTITION_PART="sys_rev"

get_boot_config() {
	local BOOT_CFG_PART="/dev/$1"
	dd if="$BOOT_CFG_PART" skip=4 bs=1 count=1 2>/dev/null | \
	 hexdump -v -e '/1 "%x\n"'
}

get_dyn_magic_code() {
	dd if="$1" bs=1 count=4 2>/dev/null | hexdump -v -e '/1 "%x"'
}

# 0 = A; 1 = B
boot_config_name() {
	local cfg="$1"

	if [ "$cfg" == "0" ]; then
		echo "A"
	elif [ "$cfg" == "1" ]; then
		echo "B"
	else
		echo "unknown"
	fi
}

change_ubi_part() {
	local cfg="$1"

	if [ "$cfg" == "0" ]; then
		CI_KERNPART="boot_b"
		CI_UBIPART="rootfs_b"
	elif [ "$cfg" == "1" ]; then
		CI_KERNPART="boot_a"
		CI_UBIPART="rootfs_a"
	else
		echo "Unable to detect boot config!"
		exit 1
	fi
}

set_boot_config() {
	local cfg="$1"
	local BOOT_CFG_PART="/dev/mtdblock$2"

	if [ "$cfg" == "0" ]; then
		cfg="1"
	elif [ "$cfg" == "1" ]; then
		cfg="0"
	fi

	printf "\x0$cfg" | dd of="$BOOT_CFG_PART" bs=1 \
			    seek=4 count=1 conv=notrunc 2>/dev/null
}

nand_upgrade_kernel() {
	local tar_file="$1"
	local kernel_mtd="$(find_mtd_index $CI_KERNPART)"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	local kernel_length=`(tar xf $tar_file ${board_dir}/kernel -O | wc -c) 2> /dev/null`

	[ "$kernel_length" != 0 -a -n "$kernel_mtd" ] && {
		tar xf $tar_file ${board_dir}/kernel -O | mtd write - $CI_KERNPART
	}
}

nand_upgrade_rootfs() {
	local tar_file="$1"
	local rootfs_mtd="$(find_mtd_index "$CI_UBIPART")"

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}

	[ ! "$rootfs_mtd" ] && {
		CI_UBIPART="rootfs"
		rootfs_mtd="$(find_mtd_index "$CI_UBIPART")"
	}

	if [ ! "$rootfs_mtd" ]; then
		echo "cannot find mtd device $CI_UBIPART"
		umount -a
		reboot -f
	fi

	tar xf $tar_file ${board_dir}/root -C /tmp

	local mtddev="/dev/mtd${rootfs_mtd}"
	sync
	ubiformat "${mtddev}" -y -f "/tmp/${board_dir}/root"
	ubiattach -p "${mtddev}"
}

prepare_kernel() {
	local tar_dir="$1"
	local dtb_files=$(find ${tar_dir}/dtb/ -iname *mdm9607*.dtb | \
	 awk -F dtb/ '{print $NF}' | awk -F[.][d] '{print $1}')
	local pagesize="$2"
	local cmdline=$(cat ${tar_dir}/cmdline)

	echo "Attempt to generate masterDTB"

	for d in $dtb_files; do
		targets=$(echo ${d#mdm9607-}| awk '{split($0,a, "/"); print a[1]}')
		cat ${tar_dir}/zImage ${tar_dir}/dtb/$d.dtb > ${tar_dir}/dtb/dtb-zImage-$targets
		/usr/bin/dtbtool ${tar_dir}/dtb/ \
		 -s ${pagesize} \
		 -o ${tar_dir}/masterDTB \
		 -p /usr/bin/
	done

	echo "Attempt to generate bootable kernel image"

	/usr/bin/mkbootimg \
	 --kernel ${tar_dir}/zImage \
	 --dt ${tar_dir}/masterDTB \
	 --ramdisk NONE \
	 --cmdline "${cmdline}" \
	 --pagesize ${pagesize} \
	 --base 0x80000000 \
	 --tags-addr 0x81E00000 \
	 --output ${tar_dir}/kernel

	[ $? -ne 0 ] && {
		echo "Unable generate mkbootimg file"
		exit 1
	}

	rm -rf ${tar_dir}/dtb
	rm ${tar_dir}/masterDTB
	rm ${tar_dir}/zImage
	rm ${tar_dir}/cmdline
}

prepare_rootfs() {
	local board_dir="$1"
	local blocksize="$2"
	local pagesize="$3"
	local subpagesize="$4"

	echo "Attempt to ubinize rootfs image with:"
	echo " -blocksize: ${blocksize}KiB"
	echo " -pagesize: ${pagesize}"
	echo " -subpagesize: ${subpagesize}"

	/usr/bin/ubinize-image.sh \
		/tmp/${board_dir}/root \
		/tmp/${board_dir}/root.tmp \
		-p "${blocksize}KiB" \
		-m "${pagesize}" \
		-s "${subpagesize}"

	[ $? -ne 0 ] && {
		echo "Unable to ubinize rootfs image"
		exit 1
	}

	mv /tmp/${board_dir}/root.tmp /tmp/${board_dir}/root
}

prepare_image() {
	. /usr/share/libubox/jshn.sh

	local tar_file="$1"
	local blocksize pagesize subpagesize

	json_load "$(cat /etc/board.json)"
	json_select nand
	json_get_vars blocksize pagesize subpagesize

	dd if="$tar_file" of="$tar_file.tmp" bs=1M skip=4 \
	 iflag=skip_bytes 2>/dev/null
	mv "$tar_file.tmp" $tar_file

	local board_dir=$(tar tf $tar_file | grep -m 1 '^sysupgrade-.*/$')
	board_dir=${board_dir%/}
	local board_name=${board_dir//sysupgrade-}

	tar xf $tar_file -C /tmp
	[ $? -ne 0 ] && {
		echo "Unable to untar firmware file"
		exit 1
	}

	rm $tar_file

	prepare_kernel "/tmp/${board_dir}" ${pagesize}
	prepare_rootfs "sysupgrade-${board_name}" ${blocksize} ${pagesize} ${subpagesize}

	/usr/bin/sysupgrade-tar.sh \
		--board "${board_name}" \
		--kernel /tmp/${board_dir}/kernel \
		--rootfs /tmp/${board_dir}/root \
		$tar_file

	[ $? -ne 0 ] && {
		echo "Unable to re-generate sysupgrade-tar image"
		exit 1
	}

	rm -rf /tmp/${board_dir}
}

has_embargo_hw() {
	[ -n "$(grep "$EMBARGO_PARTITION_PART" /proc/mtd | awk -F: '{print $1}')" ]
}


get_phy_id() {
	cut -c 3-6 /sys/devices/platform/7c40000.qcom,emac/mdio_bus/7c40000.qcom,emac/7c40000.qcom,emac:00/phy_id 2>/dev/null
}

get_econet_phy_ver() {
	local path="/sys/kernel/debug/en8801_phy/version"
	[ -f "$path" ] && cat "$path"
}




platform_check_hw_support() {
	local board="$(mnf_info -n | cut -c 1-6)"
	local hwver="$(mnf_info -H )"

	prepare_metadata_hw_mods "$1" && {
		[ "$(get_phy_id)" = "4f51" ] && { ! find_hw_mod "motorcomm_phy"; } && {
			echo "Motorcomm PHY detected but fw does not support it"
			return 1
		}
		[ "$(get_phy_id)" = "03a2" ] && { ! find_hw_mod "econet_phy"; } && {
			echo "Econet PHY detected but fw does not support it"
			return 1
		}
		[ "$board" = "TRB143" ] && { ! find_hw_mod "trb143"; } && {
			echo "TRB143 device detected but fw does not support it"
			return 1
		}
		[ "$board" = "TRB145" ] && [ $hwver -gt 7 ] && { ! find_hw_mod "trb145_hw_txen"; } && {
			echo "TRB145 V08 device detected but fw does not support it"
			return 1
		}
		has_embargo_hw && { ! find_hw_mod "quectel_embargo"; } && {
			echo "Embargo module detected, but fw does not support it"
			return 1
		}
		[ "$(get_phy_id)" = "03a2" ] && [ "$(get_econet_phy_ver)" = "4" ] && { ! find_hw_mod "econet_phy_v4"; } && {
			echo "Econet PHY V4 detected but fw does not support it"
			return 1
		}
		return 0
	}
	return 1
}

platform_check_image() {
	. /usr/share/libubox/jshn.sh

	local tar_file="$1"
	local filesize tmpsize

	# this is not dynamic upgrade file
	[ "$(get_dyn_magic_code $tar_file)" != "c0de104" ] && {
		return 0
	}

	# check size of /tmp; for generation it should be double the firmware
	# file is
	tmpsize=$(df | grep /tmp | awk '{print $4}')
	filesize=$(ls -l "$tar_file" | awk '{print $5}')
	# double the size and convert to KB
	filesize=$(($filesize * 2 / 1024))

	if [ $tmpsize -lt $filesize ]; then
		fwtool_msg "Insufficient free space in /tmp/" "15"
		echo "Not enough free space in /tmp file system!"
		echo "Please clean up /tmp mount point."
		return 1
	fi

	# check if flash info exists
	local blocksize pagesize subpagesize

	json_load "$(cat /etc/board.json)"
	json_select nand
	json_get_vars blocksize pagesize subpagesize

	[ -n "$blocksize" ] || return 1
	[ -n "$pagesize" ] || return 1
	[ -n "$subpagesize" ] || return 1

	return 0
}

platform_do_upgrade() {
	. /lib/upgrade/nand.sh

	# this is not dynamic upgrade file
	[ "$(get_dyn_magic_code $1)" == "c0de104" ] && {
		echo "Dynamic upgrade file detected"
		prepare_image "$1"
	}

	local mtdpart=$(grep "boot_config" /proc/mtd | awk -F: '{print $1}')
	local b_num=$(get_boot_config $mtdpart)
	local b_name=$(boot_config_name $b_num)

	echo "Detected boot \"$b_name\" configuration"

	# change target ubi part names
	change_ubi_part $b_num

	echo "New UBI part names will be after reboot:"
	echo "kernel: $CI_KERNPART; rootfs: $CI_UBIPART"

	nand_upgrade_kernel "$1"
	nand_upgrade_rootfs "$1"

	# update boot config only after successful flash
	set_boot_config $b_num ${mtdpart##mtd}

	nand_do_upgrade_success
}
