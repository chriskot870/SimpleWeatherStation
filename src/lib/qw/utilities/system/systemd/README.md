
This directory contains files to access the systemd daemon through the sd-bus.
It is kind of complicated. Below are the various reference sites about the topic.

Information about sd-bus library can be found here.
https://www.freedesktop.org/wiki/Software/dbus/
https://www.freedesktop.org/software/systemd/man/latest/sd-bus.html
https://0pointer.net/blog/the-new-sd-bus-api-of-systemd.html
https://manpages.ubuntu.com/manpages/bionic/man1/busctl.1.html
https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.systemd1.html
 
You can get a list of the methods and properties for systemd by doing
$ busctl introspect org.freedesktop.systemd1 /org/freedesktop/systemd1

If you look in the man page https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.systemd1.html
you will find the following systemd object types:

The Manager Object
Unit Objects
Service Unit Objects
Socket Unit Objects
Target Unit Objects
Device Unit Objects
Mount Unit Objects
Automount Unit Objects
Timer Unit Objects
Swap Unit Objects
Path Unit Objects
Slice Unit Objects
Scope Unit Objects
Job Objects

For this application we only need the Manager, Unit, and Service Unit Objects. Each
of these objects contain their own methods, signals, and properties. This directory
contains objects for each of the items needed for this application. Each object has
functions to get the properties or call methods needed by this application.

systemd_manager.cpp
systemd_unit.cpp
systemd_service.cpp