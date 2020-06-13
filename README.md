This is a fork of libsigrok which adds _extremely_ primitive Pololu SLO-scope support.

The Pololu SLO-scope is a very basic oscilliscope included as a bonus feature on the Pololu AVR programmer.

The very basics work, but this has a few big problems:

  1. PulseView hangs on "Stop"
  2. Only one channel works
  3. Can't select the "1 analog, 1 digital" mode
  4. Device isn't freed, so you'll need to disconnect and reconnect USB to use it again

Here's the slightly bodgy process I used to quickly test this:

  1. Install PulseView and its dependencies through your package manager
  2. Find the version of libsigrok it's using in settings
  3. Undo the last commit on this repo (which actually adds the SLO-scope support) into your staging
  4. Checkout the tag for the version of libsigrok your PulseView needs
  5. Build sigrok as normal, and use `sudo make install` to put it in `/usr/local/lib`
  6. Run PulseView with `LD_LIBRARY_PATH=/usr/local/lib pulseview`

------------

Here's the original sigrok readme:
```
-------------------------------------------------------------------------------
README
-------------------------------------------------------------------------------

The sigrok project aims at creating a portable, cross-platform,
Free/Libre/Open-Source signal analysis software suite that supports various
device types (such as logic analyzers, oscilloscopes, multimeters, and more).

libsigrok is a shared library written in C which provides the basic API
for talking to hardware and reading/writing the acquired data into various
input/output file formats.


Status
------

libsigrok is in a usable state and has had official tarball releases.

While the API can change from release to release, this will always be
properly documented and reflected in the package version number and
in the shared library / libtool / .so-file version numbers.

However, there are _NO_ guarantees at all for stable APIs in git snapshots!
Distro packagers should only use released tarballs (no git snapshots).


Requirements
------------

Requirements for the C library:

 - git (only needed when building from git)
 - gcc (>= 4.0) or clang
 - make
 - autoconf >= 2.63 (only needed when building from git)
 - automake >= 1.11 (only needed when building from git)
 - libtool (only needed when building from git)
 - pkg-config >= 0.22
 - libglib >= 2.32.0
 - libzip >= 0.10
 - libserialport >= 0.1.1 (optional, used by some drivers)
 - librevisa >= 0.0.20130412 (optional, used by some drivers)
 - libusb-1.0 >= 1.0.16 (optional, used by some drivers)
 - hidapi >= 0.8.0 (optional, used for some HID based "serial cables")
 - bluez/libbluetooth >= 4.0 (optional, used for Bluetooth/BLE communication)
 - libftdi1 >= 1.0 (optional, used by some drivers)
 - libgpib (optional, used by some drivers)
 - libieee1284 (optional, used by some drivers)
 - libgio >= 2.32.0 (optional, used by some drivers)
 - check >= 0.9.4 (optional, only needed to run unit tests)
 - doxygen (optional, only needed for the C API docs)
 - graphviz (optional, only needed for the C API docs)

Requirements for the C++ bindings:

 - libsigrok >= 0.4.0 (the libsigrok C library, see above)
 - A C++ compiler with C++11 support (-std=c++11 option), e.g.
   - g++ (>= 4.8.1)
   - clang++ (>= 3.3)
 - autoconf-archive (only needed when building from git)
 - doxygen (required for building the bindings, not only for C++ API docs!)
 - graphviz (optional, only needed for the C++ API docs)
 - Python (2 or 3) executable (development files are not needed)
 - glibmm-2.4 (>= 2.32.0)

Requirements for the Python bindings:

 - libsigrokcxx >= 0.4.0 (the libsigrok C++ bindings, see above)
 - Python >= 2.7 or Python >= 3 (including development files!)
 - Python setuptools (for Python 2 or 3)
 - pygobject >= 3.0.0 (for Python 2 or 3), a.k.a python-gi
 - numpy (for Python 2 or 3)
 - SWIG >= 2.0.0
 - doxygen (optional, only needed for the Python API docs)
 - graphviz (optional, only needed for the Python API docs)
 - doxypy (optional, only needed for the Python API docs)

Requirements for the Ruby bindings:

 - libsigrokcxx >= 0.4.0 (the libsigrok C++ bindings, see above)
 - Ruby >= 1.9.3 (including development files!)
 - SWIG >= 3.0.8
 - YARD (optional, only needed for the Ruby API docs)

Requirements for the Java bindings:

 - libsigrokcxx >= 0.4.0 (the libsigrok C++ bindings, see above)
 - SWIG >= 2.0.0
 - Java JDK (for JNI includes and the javac/jar binaries)
 - doxygen (optional, only needed for the Java API docs)
 - graphviz (optional, only needed for the Java API docs)


Building and installing
-----------------------

In order to get the libsigrok source code and build it, run:

 $ git clone git://sigrok.org/libsigrok
 $ cd libsigrok
 $ ./autogen.sh
 $ ./configure
 $ make

For installing libsigrok:

 $ make install

See INSTALL or the following wiki page for more (OS-specific) instructions:

 http://sigrok.org/wiki/Building

Please also check the following wiki page in case you encounter any issues:

 http://sigrok.org/wiki/Building#FAQ


Device-specific issues
----------------------

Please check README.devices for some notes and hints about device- or
driver-specific issues to be aware of.


Firmware
--------

Some devices supported by libsigrok need a firmware to be uploaded before the
device can be used. See README.devices for details.


Copyright and license
---------------------

libsigrok is licensed under the terms of the GNU General Public License
(GPL), version 3 or later.

While some individual source code files are licensed under the GPLv2+, and
some files are licensed under the GPLv3+, this doesn't change the fact that
the library as a whole is licensed under the terms of the GPLv3+.

Please see the individual source files for the full list of copyright holders.


Mailing list
------------

 https://lists.sourceforge.net/lists/listinfo/sigrok-devel


IRC
---

You can find the sigrok developers in the #sigrok IRC channel on Freenode.


Website
-------

 http://sigrok.org/wiki/Libsigrok

```