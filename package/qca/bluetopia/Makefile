#
# Copyright (c) 2015 Qualcomm Atheros, Inc.
#
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=bluetopia
PKG_VERSION:=4.2.1.c1_3
PKG_DIST_VERSION:=4.2.1.c1
PKG_RELEASE:=1
PKG_FOLDER=BLUETOPIA

#
# Specify git commit hash in this variable if you want to test a commit which
# is not pointed by any tag. Otherwise, specify tag in PKG_VERSION and leave
# blank to PKG_SOURCE_TREEISH.
#
#PKG_GIT_TREEISH:=c37bc444b7180841be49ec01257b9d2480968f5f
PKG_GIT_TREEISH:=06550adb1ab16997078846a492108715b169d009
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
#PKG_SOURCE_PROTO:=git
#PKG_SOURCE_URL:=$(if $(GIT_HOME),$(GIT_HOME)/$(CONFIG_BLUETOPIA_GIT_REPOSITORY),$(TOPDIR)/git_home/$(CONFIG_BLUETOPIA_GIT_REPOSITORY))
#PKG_SOURCE_VERSION:=$(if $(PKG_SOURCE_TREEISH),$(PKG_SOURCE_TREEISH),$(PKG_VERSION))
#PKG_SOURCE:=$(PKG_NAME)-$(PKG_SOURCE_VERSION).tar.bz2

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_DIST_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_DIST_VERSION)

# Bluetopia Root Paths
export BLUETOPIA_SOURCE_PATH:=$(PKG_BUILD_DIR)/bt_host/Build
export BLUETOPIAPM_SOURCE_PATH:=$(PKG_BUILD_DIR)/bt_host_pm/Build
export BLUETOPIA_ROOT_PATH:=$(PKG_DIST_DIR)/Bluetopia_Dist
export BTPM_DIST_PATH:=$(PKG_DIST_DIR)/BluetopiaPM_Dist

# Bluetopia Core Include and Library Path
BLUETOPIA_INCLUDE_PATH:=$(BLUETOPIA_ROOT_PATH)/include
export BLUETOPIA_LIB_PATH:=$(BLUETOPIA_ROOT_PATH)/lib

# Bluetopia Debug Include and Library Path
BLUETOPIA_DEBUG_INCLUDE_PATH:=$(BLUETOPIA_ROOT_PATH)/debug/include
BLUETOPIA_DEBUG_LIB_PATH:=$(BLUETOPIA_ROOT_PATH)/debug/lib

# Bluetopia Profiles Root Path
export BLUETOPIA_PROFILE_ROOT_PATH:=$(BLUETOPIA_ROOT_PATH)/profiles

# Bluetopia VSER Include and Library Path
BLUETOPIA_VSER_INCLUDE_PATH:=$(BLUETOPIA_ROOT_PATH)/VSER/include
BLUETOPIA_VSER_LIB_PATH:=$(BLUETOPIA_ROOT_PATH)/VSER/lib

# Bluetopia VNET Include and Library Path
BLUETOPIA_VNET_INCLUDE_PATH:=$(BLUETOPIA_ROOT_PATH)/VNET/include
BLUETOPIA_VNET_LIB_PATH:=$(BLUETOPIA_ROOT_PATH)/VNET/lib

# Bluetopia SBC Include and Library Path
BLUETOPIA_SBC_INCLUDE_PATH:=$(BLUETOPIA_ROOT_PATH)/SBC/include
BLUETOPIA_SBC_LIB_PATH:=$(BLUETOPIA_ROOT_PATH)/SBC/lib

# Bluetopia Simple XML Parser Path
export BLUETOPIA_XML_PATH:=$(BLUETOPIA_ROOT_PATH)/XML/SS1SXMLP

# Bluetopia Distribution Path
export BLUETOPIA_PATH:=$(BLUETOPIA_ROOT_PATH)


include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=QCA
  CATEGORY:=QCA Proprietary software
  TITLE:=$(1) - QCA Bluetopia
  DEPENDS:=+libpthread 
endef

define Package/$(PKG_NAME)/description
  This package contains Bluetopia PM and Bluetopia binaries
endef

define Package/$(PKG_NAME)/config
	menu "$(PKG_NAME) git configuration"
		depends on PACKAGE_$(PKG_NAME)
		source "$(SOURCE)/Config.in"
	endmenu
endef

MAKE_FLAGS="CC=$$(TARGET_CC)" \
	   "GLOBLDFLAGS=$$(TARGET_LDFLAGS)" \
	   "GLOBCFLAGS=$$(TARGET_CFLAGS)"

INCLDIRS = -I$(BLUETOPIA_INCLUDE_PATH)        \
	   -I$(BLUETOPIA_DEBUG_INCLUDE_PATH)  \
	   -I$(BLUETOPIA_VSER_INCLUDE_PATH)   \
	   -I$(BLUETOPIA_VNET_INCLUDE_PATH)   \
	   -I$(BLUETOPIA_SBC_INCLUDE_PATH) 

LIBDIRS = -L$(BLUETOPIA_LIB_PATH)          \
	  -L$(BLUETOPIA_DEBUG_LIB_PATH)    \
	  -L$(BLUETOPIA_VSER_LIB_PATH)     \
	  -L$(BLUETOPIA_VNET_LIB_PATH)     \
	  -L$(BLUETOPIA_SBC_LIB_PATH)      

PM_MAKE_FLAGS="CC=$$(TARGET_CC)" \
	   "GLOBLDFLAGS=$$(TARGET_LDFLAGS)" \
	   "GLOBCFLAGS=$$(TARGET_CFLAGS)" \
	   "BLUETOPIA_INCLUDE_PATH=$$(BLUETOPIA_ROOT_PATH)/include"

define Build/Prepare
	$(CP) $(GIT_HOME)/bluetopia.git/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef


define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/PS_KEY_CSR8811.txt $(1)/usr/bin
	$(INSTALL_BIN) ./files/bluetopia.init $(1)/etc/init.d/bluetopia
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/sample/LinuxHCI/LinuxHCI $(1)/usr/bin/
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/sample/LinuxL2CAP/LinuxL2CAP $(1)/usr/bin/
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/sample/LinuxSCO/LinuxSCO $(1)/usr/bin/
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/sample/LinuxSDP/LinuxSDP $(1)/usr/bin/
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/sample/SS1BTVEN/libSS1BTVEN.a $(1)/usr/lib/
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/profiles_gatt/sample/LinuxSPPLE/LinuxSPPLE $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/profiles/GATT/libSS1BTGAT.a  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/profiles/GATT/libSS1BTGAT_LE.a  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/btpsvend/libBTPSVEND.a  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/btpsfile/libBTPSFILE.a  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host/Build/Linux/ss1btps/libSS1BTPS.a  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host_pm/Build/Linux/ss1btpm/server/SS1BTPM  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host_pm/Build/Linux/ss1btpm/client/libSS1BTPM.a  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host_pm/Build/Linux/sample/LinuxSPPLE_PM/LinuxSPPLE_PM  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host_pm/Build/Linux/sample/LinuxGATM/LinuxGATM_CLT  $(1)/usr/bin
	$(INSTALL_BIN) \
	    $(PKG_BUILD_DIR)/bt_host_pm/Build/Linux/ss1btpm/ntgbpm/ntgbpm  $(1)/usr/bin
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
