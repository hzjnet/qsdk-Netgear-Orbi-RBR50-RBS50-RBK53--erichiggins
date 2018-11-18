# 
#  Copyright (C) 2007-2009 Delta Networks Inc.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=net-scan-ext
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

CONFIG_NETSCAN-EXT_GIT_TREEISH=dd79a142b75ab41f29c013dda47abbe164a8cd6a
CONFIG_NETSCAN-EXT_GIT_REPOSITORY="net-scan.git"

PKG_GIT_TREEISH:=$(CONFIG_NETSCAN-EXT_GIT_TREEISH)

LOCAL_PKG_NAME_EXT:=net-scan-ext

include $(INCLUDE_DIR)/package.mk

define Package/net-scan-ext
	SECTION:=utils
	CATEGORY:=Network
	DEPENDS:=@TARGET_ipq806x
	TITLE:=Scanning the attached devices according to NETGEAR's spec.
	DEPENDS:=datalib
endef


define Package/net-scan-ext/config
	menu "Configuration"
		depends on PACKAGE_net-scan-ext
		source "$(SOURCE)/Config.in"
	endmenu
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/net-scan-ext.git/* $(PKG_BUILD_DIR)/
endef

EXTRA_CFLAGS := $(EXTRA_CFLAGS) $(call qstrip,$(CONFIG_NETSCANEXT_EXTRA_FLAGS))

define Package/net-scan-ext/install
	install -d -m0755 $(1)/etc/init.d
	install -m0755 $(PKG_BUILD_DIR)/files/net-scan.init $(1)/etc/init.d/netscan
	install -m0755 $(PKG_BUILD_DIR)/files/netscan_if.init $(1)/etc/init.d/netscan_if.init
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/net-scan $(1)/usr/sbin/net-scan-ext
	install -m0755 $(PKG_BUILD_DIR)/dev-scan $(1)/usr/sbin/dev-scan-ext
	$(call Dni_Install_Ext,$(LOCAL_PKG_NAME_EXT),$(1))
endef

$(eval $(call BuildPackage,net-scan-ext))
