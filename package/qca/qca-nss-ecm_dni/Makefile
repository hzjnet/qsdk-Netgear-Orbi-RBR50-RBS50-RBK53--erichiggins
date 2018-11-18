include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-nss-ecm_dni
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

include $(INCLUDE_DIR)/local-development.mk
ifeq ($(DUMP)$(PKG_VERSION),)
  PKG_VERSION:=$(shell git ls-remote $(PKG_SOURCE_URL) $(PKG_BRANCH) | cut -b -7)
endif
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=$(PKG_VERSION)

LOCAL_VARIANT=$(patsubst qca-nss-ecm_dni-%,%,$(patsubst qca-nss-ecm_dni-%,%,$(BUILD_VARIANT)))

LOCAL_PKG_NAME_EXT:=qca-nss-ecm_dni

include $(INCLUDE_DIR)/package.mk

ifeq ($(CONFIG_QCA_NSS_ECM_EXAMPLES_PCC),y)
	FILES_EXAMPLES:=$(PKG_BUILD_DIR)/examples/ecm_pcc_test.ko
endif

define KernelPackage/qca-nss-ecm_dni/Default
  SECTION:=kernel
  CATEGORY:=Kernel modules
  SUBMENU:=Network Support
  DEPENDS:=@TARGET_ipq806x +kmod-qca-nss-drv_dni +iptables-mod-extra +kmod-ipt-conntrack +kmod-ipv6 +kmod-shortcut-fe_dni-drv +kmod-l2tp +kmod-pppol2tp +kmod-bonding +kmod-pptp @!LINUX_3_18
  TITLE:=QCA NSS Enhanced Connection Manager (ECM)
  FILES:=$(PKG_BUILD_DIR)/*.ko $(PKG_BUILD_DIR)/circle_pcc/circle_pcc.ko
  KCONFIG:=CONFIG_BRIDGE_NETFILTER=y
  MENU:=1
endef

define KernelPackage/qca-nss-ecm_dni/Description/Default
This package contains the QCA NSS Enhanced Connection Manager
endef

define KernelPackage/qca-nss-ecm_dni/Default/install
	$(INSTALL_DIR) $(1)/etc/firewall.d $(1)/etc/init.d $(1)/usr/bin $(1)/lib/netifd/offload $(1)/etc/config
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/qca-nss-ecm.firewall $(1)/etc/firewall.d/qca-nss-ecm
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/qca-nss-ecm.init $(1)/etc/init.d/qca-nss-ecm
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/ecm_dump.sh $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/on-demand-down $(1)/lib/netifd/offload/on-demand-down
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/qca-nss-ecm.uci $(1)/etc/config/ecm
endef

define KernelPackage/qca-nss-ecm_dni
  $(call KernelPackage/qca-nss-ecm_dni/Default)
  AUTOLOAD:=$(call AutoLoad,52,ecm)
endef

define KernelPackage/qca-nss-ecm_dni/Description
  $(call KernelPackage/qca-nss-ecm_dni/Description/Default)
endef

define KernelPackage/qca-nss-ecm_dni/install
$(call KernelPackage/qca-nss-ecm_dni/Default/install, $(1))
	$(INSTALL_DIR) $(1)/etc/uci-defaults $(1)/etc/sysctl.d
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/qca-nss-ecm.defaults $(1)/etc/uci-defaults/99-qca-nss-ecm
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/files/qca-nss-ecm.sysctl $(1)/etc/sysctl.d/qca-nss-ecm.conf

	# install service by default
	$(INSTALL_DIR) $(1)/etc/rc.d
	$(LN) ../init.d/qca-nss-ecm $(1)/etc/rc.d/S70qca-nss-ecm
	$(call Dni_Install_Ext,$(LOCAL_PKG_NAME_EXT),$(1))
endef

# Variant with additional features enabled for premium profile
define KernelPackage/qca-nss-ecm_dni-premium/Default
$(call KernelPackage/qca-nss-ecm_dni/Default)
  TITLE+= (with premium features)
  VARIANT:=premium
  DEPENDS+=+kmod-qca-hyfi-bridge_dni +kmod-nat46
endef

define KernelPackage/qca-nss-ecm_dni-premium/Description/Default
$(call KernelPackage/qca-nss-ecm_dni/Description/Default)
with the premium features enabled
endef

define KernelPackage/qca-nss-ecm_dni-premium/Default/install
$(call KernelPackage/qca-nss-ecm_dni/install)
endef

define KernelPackage/qca-nss-ecm_dni-premium
$(call KernelPackage/qca-nss-ecm_dni-premium/Default)
AUTOLOAD:=$(call AutoLoad,52,ecm)
endef

define KernelPackage/qca-nss-ecm_dni-premium/Description
$(call KernelPackage/qca-nss-ecm_dni-premium/Description/Default)
endef

define KernelPackage/qca-nss-ecm_dni-premium/install
$(call KernelPackage/qca-nss-ecm_dni/install, $(1))
endef

define KernelPackage/qca-nss-ecm_dni-noload
  $(call KernelPackage/qca-nss-ecm_dni/Default)
  PROVIDES:=kmod-qca-nss-ecm_dni
endef

define KernelPackage/qca-nss-ecm_dni-noload/Description
  $(call KernelPackage/qca-nss-ecm_dni/Description/Default)
  When selected, this package installs the driver but does not load it at init.
endef

define KernelPackage/qca-nss-ecm_dni-noload/install
$(call KernelPackage/qca-nss-ecm_dni/Default/install, $(1))
endef

define KernelPackage/qca-nss-ecm_dni-premium-noload
  $(call KernelPackage/qca-nss-ecm_dni-premium/Default)
  PROVIDES:=kmod-qca-nss-ecm_dni-premium
endef

define KernelPackage/qca-nss-ecm_dni-premium-noload/Description
  $(call KernelPackage/qca-nss-ecm_dni-premium/Description/Default)
  When selected, this package installs the driver but does not load it at init.
endef

define KernelPackage/qca-nss-ecm_dni-premium-noload/install
$(call KernelPackage/qca-nss-ecm_dni-premium/Default/install, $(1))
endef

EXTRA_CFLAGS+= \
	-I$(STAGING_DIR)/usr/include/hyfibr \
	-I$(STAGING_DIR)/usr/include/qca-mcs \
	-I$(STAGING_DIR)/usr/include/qca-nss-drv \
	-I$(STAGING_DIR)/usr/include/shortcut-fe \
	-I$(STAGING_DIR)/usr/include/nat46

ECM_MAKE_OPTS:=
ifeq ($(LOCAL_VARIANT),premium)
ECM_MAKE_OPTS+=ECM_CLASSIFIER_HYFI_ENABLE=y \
		ECM_MULTICAST_ENABLE=y
endif

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
	mkdir -p $(PKG_BUILD_DIR)/files
	$(CP) ./files/* $(PKG_BUILD_DIR)/files/
	$(call Build/Patch/Default)
	$(if $(PLATFORM_PATCH_DIR), $(call PatchDir,$(PKG_BUILD_DIR),$(PLATFORM_PATCH_DIR),))
	$(call Dni_Prepare_Ext,$(LOCAL_PKG_NAME_EXT))
endef

ifneq ($(CONFIG_PACKAGE_kmod-nat46),)
ECM_MAKE_OPTS+=ECM_INTERFACE_MAP_T_ENABLE=y
endif

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" $(strip $(ECM_MAKE_OPTS)) \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		EXAMPLES_BUILD_PCC="$(CONFIG_QCA_NSS_ECM_EXAMPLES_PCC)" \
		modules
endef

define KernelPackage/qca-nss-ecm_dni/config
$(call Package/$(PKG_NAME)/override_source_path,kmod-$(PKG_NAME))
$(call Package/$(PKG_NAME)/override_version,kmod-$(PKG_NAME),$(PKG_SUPPORTED_VERSION))
menu "ECM Configuration"
	config QCA_NSS_ECM_EXAMPLES_PCC
		bool "Build PCC usage example"
		help
			Selecting this will build the PCC classifier usage example module.
		default n
endmenu
endef

$(eval $(call KernelPackage,qca-nss-ecm_dni-noload))
$(eval $(call KernelPackage,qca-nss-ecm_dni))
$(eval $(call KernelPackage,qca-nss-ecm_dni-premium-noload))
$(eval $(call KernelPackage,qca-nss-ecm_dni-premium))
