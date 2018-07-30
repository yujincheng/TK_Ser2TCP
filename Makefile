#
# Copyright (C) 2008-2010 OpenWrt.org
# Copyright (C) 2016 fly.zhu <flyzjhz@gmail.com>
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ser2TCP
PKG_VERSION:=1.0.0
PKG_RELEASE:=1

PKG_MAINTAINER:=fly.zhu <flyzjhz@gmail.com>
PKG_LICENSE:=GPL-2.0

include $(INCLUDE_DIR)/package.mk

define Package/ser2TCP
  SECTION:=net
  CATEGORY:=Utilities
  DEPENDS:= +libpthread
  TITLE:=ser2TCP communication utility
endef


define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

# define Build/Compile
# 	CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
# 	$(MAKE) -C $(PKG_BUILD_DIR) \
# 		$(TARGET_CONFIGURE_OPTS) \
# 		LIBS="$(TARGET_LDFLAGS) -lm -lpthread"
# endef

define Package/ser2TCP/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ser2TCP $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ser2TCP.init $(1)/etc/init.d/ser2TCP
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) ./files/ser2TCP.config $(1)/etc/config/ser2TCP
	$(INSTALL_DIR) $(1)/etc/hotplug.d/iface
	$(INSTALL_DATA) ./files/ser2TCP.hotplug $(1)/etc/hotplug.d/iface/20-ser2TCP
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/controller
	$(INSTALL_DATA) ./files/ser2TCP_controller.lua $(1)/usr/lib/lua/luci/controller/ser2TCP.lua
	$(INSTALL_DIR) $(1)/usr/lib/lua/luci/model/cbi
	$(INSTALL_DATA) ./files/ser2TCP_cbi.lua $(1)/usr/lib/lua/luci/model/cbi/ser2TCP.lua
	$(INSTALL_DIR) $(1)/etc/uci-defaults
	$(INSTALL_BIN) ./files/ser2TCP.uci $(1)/etc/uci-defaults/luci-ser2TCP
endef

$(eval $(call BuildPackage,ser2TCP))
