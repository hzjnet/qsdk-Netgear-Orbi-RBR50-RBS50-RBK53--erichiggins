include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=qca-edma_dni
PKG_SOURCE_PROTO:=git
PKG_BRANCH:=master
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/qca-edma_dni
     SUBMENU:=Network Devices
     TITLE:=QCA IPQ40xx ethernet driver
     FILES:=$(LINUX_DIR)/drivers/net/ethernet/qcom/essedma/essedma.ko
     AUTOLOAD:=$(call AutoLoad,45,essedma)
endef

define KernelPackage/qca-edma_dni/description
     Kernel modules for IPQ40xx integrated ethernet adapater.
endef

Build/Compile=true

define KernelPackage/qca-edma_dni/install
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/qca-edma $(1)/etc/init.d/qca-edma
endef

$(eval $(call KernelPackage,qca-edma_dni))
