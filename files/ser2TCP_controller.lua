--[[
 Copyright (C) 2015 OpenWrt-dist
 Copyright (C) 2015 Jian Chang <aa65535@live.com>

 This is free software, licensed under the GNU General Public License v3.
 See /LICENSE for more information.
]]--

module("luci.controller.ser2TCP", package.seeall)

function index()
	if not nixio.fs.access("/etc/config/ser2TCP") then
		return
	end

	entry({"admin", "services", "ser2TCP"}, cbi("ser2TCP"), _("ser2TCP"), 60).dependent = true
end
