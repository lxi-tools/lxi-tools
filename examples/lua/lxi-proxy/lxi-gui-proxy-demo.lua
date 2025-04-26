
local lgi = require"lgi"
local Gio = lgi.Gio
local Gtk = lgi.require("Gtk", "3.0")


local dir = Gio.File.new_for_commandline_arg(arg[0]):get_parent()
local dir_name = Gio.File.get_parse_name(dir);

package.path = dir_name..'/client/?.lua;' .. package.path
require "lxi-gui-api"


local Builder = Gtk.Builder.new_from_file(dir_name.."/lxi-proxy-test.ui")
local UI      = Builder.objects

local App = Gtk.Application{
  application_id = "io.github.lxi-tools.GTK-Lxi-Proxy.Lua.Gtk3.Demo"
}

function App:on_startup()
  self:add_window(UI.window)

end

function button_click(button)
  function button:on_clicked()
    local label_button = self:get_label()

    if (lxi_proxy_detect() ~= true) then
      response.label = "lxi-proxy not started"
      return
    end

    local ip=lxi_selected_ip()

     if(ip == nul) then
        response.label = "No Instrument selected"
        return
     end

     device = lxi_connect_selected()
     if(device < 0) then
        response.label =  "error"
     else
        local result= lxi_scpi(device,label_button)

        -- Disconnect
        lxi_disconnect_selected(device)
        --response.label =  label_button..":   "..result
        response.label =  result
     end


  end
end

function App:on_activate()
  local window = self.active_window
  for id=1,2 do
   local name="button"..id
	button_click(UI[name])
  end
  response = UI["response"]
  window:present()
end

return App:run()
