# lxi-tools

[![CircleCI](https://circleci.com/gh/lxi-tools/lxi-tools/tree/master.svg?style=shield)](https://circleci.com/gh/lxi-tools/lxi-tools/tree/master)

## 1. Introduction

lxi-tools is a collection of open source software tools for managing LXI
compatible test instruments such as modern oscilloscopes, power supplies,
spectrum analyzers etc.

Features include automatic discovery of network attached test instruments,
sending SCPI commands, grabbing screenshots from supported instruments,
benchmarking SCPI message performance, scripting for automation. Both a
commandline tool and a GUI tool are available.

lxi-tools rely on liblxi for all communication.

### 1.1 What is LXI?

LAN eXtensions for Instrumentation (LXI) is a standard developed by the LXI
Consortium, an industry consortium that maintains the LXI specification and
promotes the LXI Standard. The LXI standard defines the communication protocols
for modern instrumentation and data acquisition systems using Ethernet.

Visit https://www.lxistandard.org for more details.

Please notice that lxi-tools is not affiliated with the LXI consortium - it is
an independent open source community effort.

## 2. Usage

### 2.1 lxi-gui

The lxi-gui application provides some of the same features as the commandline
tool but presents them in a modern GUI frontend.

<img src="images/lxi-gui-scpi.png" align="center">

<img src="images/lxi-gui-screenshot.png" align="center">

<img src="images/lxi-gui-benchmark.png" align="center">

<img src="images/lxi-gui-script.png" align="center">

### 2.2 lxi

The commandline interface of the lxi application is described in the output
from 'lxi --help':
```
     Usage: lxi [--version] [--help] <command> [<args>]

       -v, --version                        Display version
       -h, --help                           Display help

     Commands:
       discover [<options>]                 Search for devices
       scpi [<options>] <scpi-command>      Send SCPI command
       screenshot [<options>] [<filename>]  Capture screenshot
       benchmark [<options>]                Benchmark
       run <filename>                       Run Lua script

     Discover options:
       -t, --timeout <seconds>              Timeout (default: 3)
       -m, --mdns                           Search via mDNS/DNS-SD

     Scpi options:
       -a, --address <ip>                   Device IP address
       -p, --port <port>                    Use port (default: VXI11: 111, RAW: 5025)
       -t, --timeout <seconds>              Timeout (default: 3)
       -x, --hex                            Print response in hexadecimal
       -i, --interactive                    Enter interactive mode
       -f, --file <filename>                Run SCPI commands from file
       -r, --raw                            Use raw/TCP

     Screenshot options:
       -a, --address <ip>                   Device IP address
       -t, --timeout <seconds>              Timeout (default: 15)
       -p, --plugin <name>                  Use screenshot plugin by name
       -l, --list                           List available screenshot plugins

     Benchmark options:
       -a, --address <ip>                   Device IP address
       -p, --port <port>                    Use port (default: VXI11: 111, RAW: 5025)
       -t, --timeout <seconds>              Timeout (default: 3)
       -c, --count <count>                  Number of request messages (default: 100)
       -r, --raw                            Use raw/TCP
```

#### 2.2.1 Example - Discover LXI devices on available networks

```
     $ lxi discover
     Searching for LXI devices - please wait...

     Broadcasting on interface lo
     Broadcasting on interface eth0
         Found "RIGOL TECHNOLOGIES,DS1104Z,DS1ZA1234567890,00.04.03.SP2" on address 10.42.1.20
         Found "RIGOL TECHNOLOGIES,DP831,DP8F1234567890,00.01.14" on address 10.42.1.67
     Broadcasting on interface wlan0

     Found 2 devices
```

#### 2.2.2 Example - Send SCPI command to an instrument

```
     $ lxi scpi --address 10.42.1.20 "*IDN?"
     RIGOL TECHNOLOGIES,DS1104Z,DS1ZA1234567890,00.04.03
```
To dump response to file simply do:
```
     $ lxi scpi --address 10.42.1.20 "*IDN?" > response.txt
```

#### 2.2.3 Example - Capture screenshot from a Rigol 1000z series oscilloscope

```
     $ lxi screenshot --address 10.42.1.20 --plugin rigol-1000z
     Saved screenshot image to screenshot_10.42.1.20_2017-11-11_13:45:47.png
```

Or using plugin autodetection simply:

```
     $ lxi screenshot --address 10.42.1.20
     Loaded rigol-1000 screenshot plugin
     Saved screenshot image to screenshot_10.42.1.20_2017-11-11_13:46:02.png
```

#### 2.2.4 Example - Capture screenshot and convert it to any image format

By default the format of the captured screenshot image is dictated by which
screenshot plugin and instrument are in play. However, it is possible to write
the screenshot image to stdout and pipe it directly to other tools for image
processing.

For example, use ImageMagicks convert tool to automatically convert any
screenshot image to JPG:

```
    $ lxi screenshot --address 10.42.1.20 - | convert - screenshot.jpg
```

#### 2.2.5 Example - List available screenshot plugins

```
     $ lxi screenshot --list
                 Name   Description
         keysight-ivx   Keysight InfiniVision 2000X/3000X series oscilloscope
          rigol-1000z   Rigol DS/MSO 1000z series oscilloscope
           rigol-2000   Rigol DS/MSO 2000 series oscilloscope
         rigol-dg4000   Rigol DG 4000 series function generator
         rigol-dm3000   Rigol DM 3000 series digital multimeter
          rigol-dp800   Rigol DP 800 series power supply
            rigol-dsa   Rigol DSA 700/800 series spectrum analyzer
           rs-hmo1000   Rohde & Schwarz HMO 1000 series oscilloscope
      siglent-sdm3000   Siglent SDM 3000/3000X series digital multimeter
          siglent-sdg   Siglent SDG 1000X/2000X/6000X series waveform generator
          siglent-sds   Siglent SDS 1000X/2000X series oscilloscope
     siglent-ssa3000x   Siglent SSA 3000X series spectrum analyzer
       tektronix-2000   Tektronix DPO/MSO 2000 series oscilloscope (experimental)
```

#### 2.2.6 Example - Benchmark instrument request/response performance

```
     $ lxi benchmark --address 10.42.1.20
     Benchmarking by sending 100 ID requests. Please wait...
     Result: 24.7 requests/second
```

## 3. Installation

lxi-tools can be installed in various ways.

### 3.1 Installation using package manager

lxi-tools comes prepackaged for various GNU/Linux distributions. Please consult
your package manager tool to find and install lxi-tools.

### 3.2 Installation via snap

Install latest stable version:
```
    $ snap install lxi-tools
```
Install bleeding edge:
```
    $ snap install lxi-tools --edge
```

### 3.3 Installation from source

The latest source releases can be found [here](https://github.com/lxi-tools/lxi-tools/releases).

To compile and install successfully from source you need to first install the
following dependencies:

 * liblxi
 * libreadline
 * lua
 * bash-completion
 * libgtk
 * gtksourceview
 * libadwaita

Install steps:
```
    $ meson build
    $ meson compile -C build
    $ meson install -C build
```

See meson\_options.txt for which features to enable/disable.

Note: Please do no try to install from source if you are not familiar with
using meson.


## 4. Tested instruments

The lxi tools are tested to work successfully with the following LXI compatible
instruments:

| Instrument                        | Working features           |
|-----------------------------------|----------------------------|
| Keysight Technologies AWG 33612A  | [discover+scpi+screenshot] |
| Keysight Technologies DMM 34461A  | [discover+scpi+screenshot] |
| Keysight Technologies MSO-X 3024T | [discover+scpi+screenshot] |
| Keysight Technologies MSO-X 6004A | [discover+scpi+screenshot] |
| Kikusui Electronics PMX35-3A      | [discover+scpi]            |
| Rigol Technologies DG1062Z        | [discover+scpi+screenshot] |
| Rigol Technologies DG4062         | [discover+scpi+screenshot] |
| Rigol Technologies DG4102         | [discover+scpi+screenshot] |
| Rigol Technologies DG4162         | [discover+scpi+screenshot] |
| Rigol Technologies DL3021         | [discover+scpi]            |
| Rigol Technologies DP831          | [discover+scpi+screenshot] |
| Rigol Technologies DP832          | [discover+scpi+screenshot] |
| Rigol Technologies DM3058         | [discover+scpi]            |
| Rigol Technologies DM3068         | [discover+scpi+screenshot] |
| Rigol Technologies DS1104Z        | [discover+scpi+screenshot] |
| Rigol Technologies DS2302         | [discover+scpi+screenshot] |
| Rigol Technologies DSA815         | [discover+scpi+screenshot] |
| Rigol Technologies MSO1104Z       | [discover+scpi+screenshot] |
| Rigol Technologies MSO2302A       | [discover+scpi+screenshot] |
| Rigol Technologies MSO5074        | [discover+scpi+screenshot] |
| Rohde & Schwarz HMC 8012          | [discover+scpi+screenshot] |
| Rohde & Schwarz HMC 8043          | [discover+scpi+screenshot] |
| Rohde & Schwarz HMO 1202          | [discover+scpi+screenshot] |
| Rohde & Schwarz HMO 3054          | [scpi+screenshot]          |
| Rohde & Schwarz RTB 2004          | [discover+scpi+screenshot] |
| Rohde & Schwarz NGE 100           | [discover+scpi+screenshot] |
| Rohde & Schwarz NGM 202           | [discover+scpi+screenshot] |
| Siglent Technologies SDG1032X     | [discover+scpi+screenshot] |
| Siglent Technologies SDG2122X     | [discover+scpi+screenshot] |
| Siglent Technologies SDG6052      | [discover+scpi+screenshot] |
| Siglent Technologies SDS1202X-E   | [discover+scpi+screenshot] |
| Siglent Technologies SDS1204X-E   | [discover+scpi+screenshot] |
| Siglent Technologies SDS2304X     | [discover+scpi+screenshot] |
| Siglent Technologies SDM3045X     | [discover+scpi+screenshot] |
| Siglent Technologies SDM3055      | [discover+scpi+screenshot] |
| Siglent Technologies SDM3065X     | [discover+scpi+screenshot] |
| Siglent Technologies SPD3303X-E   | [scpi]                     |
| Siglent Technologies SSA3032X     | [discover+scpi+screenshot] |
| Tektronix TDS3034B                | [discover+scpi+screenshot] |

Note: Feel free to add your instrument(s) to the list via GitHub pull request
or simply create a GitHub issue reporting your instrument(s) and which features
work.

## 5. Contributing

lxi-tools is open source. If you want to help out with the project please feel
free to join in.

All contributions (bug reports, code, doc, ideas, etc.) are welcome.

Please use the github issue tracker and pull request features.

Also, if you find this free open source software useful please consider making
a donation:

[![Donate](https://www.paypal.com/en_US/i/btn/x-click-but21.gif)](https://www.paypal.me/lundmar)

## 6. Website

Visit https://lxi-tools.github.io

## 7. License

This code is released under BSD-3, commonly known as the 3-clause (or
"modified") BSD license.

## 8. Authors

Created and maintained by Martin Lund \<martin.lund@keep-it-simple.com>

See the AUTHORS file for full list of contributors.

## 9. Sponsors

A big thank you to the following sponsors that have donated test equipment to
support the lxi-tools open source effort:

 * Siglent Technologies - https://www.siglent.com
 * KIKUSUI Electronics Corp. - https://www.kikusui.co.jp

