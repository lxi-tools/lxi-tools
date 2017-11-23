# lxi-tools

[![Build Status](https://travis-ci.org/lxi/lxi-tools.svg?branch=master)](https://travis-ci.org/lxi/lxi-tools)

## 1. Introduction

lxi-tools is a collection of software tools for GNU/Linux systems that enables
control of LXI compatible instruments such as modern oscilloscopes, power
supplies, spectrum analyzers etc.

All features are consolidated in the 'lxi' application which provides a simple
commandline interface to discover LXI instruments, send SCPI commands, and
capture screenshots from supported LXI instruments.

lxi-tools rely on liblxi for all communication.


## 2. Usage

The commandline interface of the lxi application is described in the output
from 'lxi --help':
```
    Usage: lxi [--version] [--help] <command> [<args>]

      -v, --version                        Display version
      -h, --help                           Display help

    Commands:
      discover [<options>]                 Search for LXI devices
      scpi [<options>] <scpi-command>      Send SCPI command
      screenshot [<options>] [<filename>]  Capture screenshot

    Discover options:
      -t, --timeout <seconds>              Timeout (default: 3)
      -m, --mdns                           Search via mDNS/DNS-SD

    Scpi options:
      -a, --address <ip>                   Device IP address
      -t, --timeout <seconds>              Timeout (default: 3)
      -x, --hex                            Print response in hexadecimal
      -i, --interactive                    Enter interactive mode
      -s, --script <filename>              Run script file
      -r, --raw                            Use raw/TCP
      -p, --raw-port                       Use raw/TCP port (default: 5025)

    Screenshot options:
      -a, --address <ip>                   Device IP address
      -t, --timeout <seconds>              Timeout (default: 15)
      -p, --plugin <name>                  Use screenshot plugin by name
      -l, --list                           List available screenshot plugins
```

### 2.1 Example - Discover LXI devices on available networks

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

### 2.2 Example - Send SCPI command to an instrument

```
     $ lxi scpi --address 10.42.1.20 "*IDN?"
     RIGOL TECHNOLOGIES,DS1104Z,DS1ZA1234567890,00.04.03
```

### 2.3 Example - Capture screenshot from a Rigol 1000z series oscilloscope

```
     $ lxi screenshot --address 10.42.1.20 --plugin rigol-1000
     Saved screenshot image to screenshot_10.42.1.20_2017-11-11_13:45:47.png
```

Or using plugin autodetection it simply becomes:

```
     $ lxi screenshot --address 10.42.1.20
     Loaded rigol-1000 screenshot plugin
     Saved screenshot image to screenshot_10.42.1.20_2017-11-11_13:46:02.png
```

### 2.4 Example - List available screenshot plugins

```
     $ lxi screenshot --list
                 Name   Description
     keysight-iv2000x   Keysight InfiniVision 2000X series oscilloscopes (experimental)
          rigol-1000z   Rigol DS/MSO 1000z series oscilloscopes
           rigol-2000   Rigol DS/MSO 2000 series oscilloscopes
         rigol-dg4000   Rigol DG4000 series function generator
         rigol-dm3000   Rigol DM3000 series digital multimeter
          rigol-dp800   Rigol DP800 series power supply
         rigol-dsa700   Rigol DSA700 series spectrum analyzer
         rigol-dsa800   Rigol DSA800 series spectrum analyzer
           rs-hmo1000   Rohde & Schwarz HMO 1000 series oscilloscopes (experimental)
     siglent-sds1000x   Siglent SDS 1000X series oscilloscopes (experimental)
     siglent-ssa3000x   Siglent SSA 3000X series spectrum analyzers (experimental)
       tektronix-2000   Tektronix DPO/MSO 2000 series oscilloscopes (experimental)
```

## 3. Installation

The latest release version can be downloaded from https://lxi.github.io

### 3.1 Installation using release tarball

Install steps:
```
    $ ./configure
    $ make
    $ make install
```

Note: lxi-tools depends on liblxi so you need to go install liblxi first.

### 3.2 Installation using package

lxi-tools comes prepackaged for various GNU/Linux distributions. Visit
https://lxi.github.io to see list of supported distributions.


## 4. Tested instruments

The commandline lxi tool is tested to work successfully with the following LXI
compatible instruments:

| Instrument                    | Working features           |
|-------------------------------|----------------------------|
| Rigol Technologies DG4102     | (discover+scpi+screenshot) |
| Rigol Technologies DG4162     | (discover+scpi+screenshot) |
| Rigol Technologies DP831      | (discover+scpi+screenshot) |
| Rigol Technologies DP832      | (discover+scpi+screenshot) |
| Rigol Technologies DM3068     | (discover+scpi+screenshot) |
| Rigol Technologies DS1104Z    | (discover+scpi+screenshot) |
| Rigol Technologies DS2302     | (discover+scpi+screenshot) |
| Rigol Technologies DSA815     | (discover+scpi+screenshot) |
| Rigol Technologies MSO2302A   | (discover+scpi+screenshot) |
| Siglent Technologies SSA3032X | (discover+scpi)            |


Note: Not all instruments work with the screenshot feature because the
instruments themselves provide no mechanism to download screenshots remotely.

Note: Feel free to add your instrument(s) to the list via GitHub pull request
or simply create a GitHub issue reporting your instrument(s) and which features
work.

## 5. Contributing

lxi-tools is open source. If you want to help out with the project please join
in.

All contributions (bug reports, code, doc, ideas, etc.) are welcome.

Please use the github issue tracker and pull request features.

Also, if you find this free open source software useful please consider making
a donation:

[![Donate](https://www.paypal.com/en_US/i/btn/x-click-but21.gif)](https://www.paypal.me/lundmar)


## 6. Website

Visit https://lxi.github.io


## 7. License

This code is released under BSD-3, commonly known as the 3-clause (or
"modified") BSD license.


## 8. Authors

Created by Martin Lund \<martin.lund@keep-it-simple.com>

See the AUTHORS file for full list of authors.
