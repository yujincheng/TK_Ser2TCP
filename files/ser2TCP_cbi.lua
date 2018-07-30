--[[
 Copyright (C) 2015 Jian Chang <aa65535@live.com>

 This is free software, licensed under the GNU General Public License v3.
 See /LICENSE for more information.
]]--

m = Map("ser2TCP", translate("ser2TCP"),
	translate("设置串口转TCP连接"))

s = m:section(TypedSection, "ser2TCP", translate("ser2TCP 设置"))
s.anonymous   = true
s.addremove   = true

o = s:option(Flag, "enabled", translate("Enable"))
o.default     = o.enabled
o.rmempty     = false

o = s:option(Value, "server_port", translate("服务器端口"))
o.placeholder = 9999
o.datatype    = "port"
o.rmempty     = false

o = s:option(Value, "server_addr", translate("服务器地址"))
o.rmempty     = false

return m
