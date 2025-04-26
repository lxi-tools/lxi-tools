# lxi-proxy

## 1. Introduction

Lxi Proxy extends lxi-tools with ability to control instruments outside lxi-tools. Lxi tools is still single point of entry to send commnds to instruments.\
Proxy is written in lua.  On PC existing lua scrips should work with little modification to include required client lua script.\
With this you get possibillity to run live-view and also to control instrument trough GUI. GUI development is responsibilty of lxi-proxy user. Here we provide just simple example.


## 2. How it works

In lxi-tools you run script lxi-proxy/server/lxi-proxy.lua which opens UDP connection on port 20040.\
On PC side you run Lua script which include client/lxi-gui-api.lua to send lxi commands to proxy.


## 3. What is needed

Currenty only tested on linux.
In linux you need to install:

```
sudo apt install lua5.3 lua-lgi lua-socket lua-cjson libgtk-3-dev
```

## 4. Run example

In Lxi-gui run script

```
Lxi-GUI->Script->Open   and open file lxi-proxy/server/lxi-proxy.lua
Lxi-GUI->Script->Run
```

On PC simply run in terminal:

```
lua5.3 lxi-proxy/connect-selected.lua

or

lua5.3 lxi-proxy/lxi-gui-proxy-demo.lua

or..

```

## 5. Why GTK-3 example

In Ubuntu/Debian old package of lua-lgi is provided. It doesn't come with support for lua5.4 and GTK-4 \
To run GTK-4 application lua5.4 is required.You can compile last lua-lgi yourself to get lua5.4 and GTK-4 support.



## 6. Addtional info for GUI development with lua

https://github.com/Miqueas/Lua-GTK-Examples.git

https://github.com/Miqueas/GTK-Examples.git