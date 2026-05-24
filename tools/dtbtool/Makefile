# 
# Copyright (C) 2007-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=dtbtool

PKG_LICENSE:=BSD-3-Clause-custom

include $(INCLUDE_DIR)/host-build.mk
include $(INCLUDE_DIR)/gpl.mk

define Host/Compile
	$(HOSTCC) $(HOST_CFLAGS) -o $(HOST_BUILD_DIR)/dtbtool dtbtool.c
endef

define Host/Install
	$(CP) $(HOST_BUILD_DIR)/dtbtool $(STAGING_DIR_HOST)/bin/
endef

define Host/Clean
	rm -f $(STAGING_DIR_HOST)/bin/dtbtool
endef

$(eval $(call HostBuild))
