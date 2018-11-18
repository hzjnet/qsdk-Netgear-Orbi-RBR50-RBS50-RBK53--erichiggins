# 
#  Copyright (C) 2007-2009 Delta Networks Inc.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=net-scan
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

CONFIG_NETSCAN_GIT_TREEISH=efd7d3dc2fbf6b8646fbfe99d25e24a8b98a56fd
CONFIG_NETSCAN_GIT_REPOSITORY="net-scan.git"

PKG_GIT_TREEISH:=$(CONFIG_NETSCAN_GIT_TREEISH)

LOCAL_PKG_NAME_EXT:=net-scan

include $(INCLUDE_DIR)/package.mk

define Package/net-scan
	SECTION:=utils
	CATEGORY:=Network
	DEPENDS:=@TARGET_ipq806x +datalib
	TITLE:=Scanning the attached devices according to NETGEAR's spec.
endef

TARGET_CFLAGS += -DDGC_MODULE_NAME="\\\"$(call qstrip,$(CONFIG_DGC_MODULE_NAME))\\\""

define Package/net-scan/config
	menu "Configuration"
		depends on PACKAGE_net-scan
		source "$(SOURCE)/Config.in"
	endmenu
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/net-scan.git/* $(PKG_BUILD_DIR)/
endef

EXTRA_CFLAGS := $(EXTRA_CFLAGS) $(call qstrip,$(CONFIG_NETSCAN_EXTRA_FLAGS))

define Package/net-scan/install
	install -d -m0755 $(1)/etc/init.d
	install -m0755 $(PKG_BUILD_DIR)/files/net-scan.init $(1)/etc/init.d/netscan
	install -m0755 $(PKG_BUILD_DIR)/files/netscan_if.init $(1)/etc/init.d/netscan_if.init
	install -m0755 $(PKG_BUILD_DIR)/files/satellite_ctype.sh $(1)/etc/init.d/satellite_ctype
	install -m0755 $(PKG_BUILD_DIR)/files/send_soap $(1)/etc/
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/net-scan $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/dev-scan $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/UA_Parser $(1)/usr/sbin/
	install -m0755 $(PKG_BUILD_DIR)/devices_info $(1)/usr/sbin/
	install -d -m0755 $(1)/etc
	install -m0755 $(PKG_BUILD_DIR)/files/oui.csv $(1)/etc/oui.csv
	install -m0755 $(PKG_BUILD_DIR)/files/mam.csv $(1)/etc/mam.csv
	install -m0755 $(PKG_BUILD_DIR)/files/oui36.csv $(1)/etc/oui36.csv
	install -m0755 $(PKG_BUILD_DIR)/files/UA_String_Match_Table.txt $(1)/etc/UA_String_Match_Table.txt
	install -m0755 $(PKG_BUILD_DIR)/files/device_type_table.json $(1)/etc/device_type_table.json
	$(call Dni_Install_Ext,$(LOCAL_PKG_NAME_EXT),$(1))
endef

$(eval $(call BuildPackage,net-scan))
