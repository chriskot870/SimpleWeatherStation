This discusses build host issues.

We want to keep a list of the packages that were installed on the build host.

To get a list of the packages installed on the system you can use the following command

```
$ sudo apt update
$ apt list --installed > pkgs_installed.lst
```

I haven't quite figured out this file format yet, but it seems to contain the
package name, the version, and wether it was automatic or not.

You can also use the apt-mark application to see the list of packages that were
manually installed:

```
$ apt-mark showmanual > pkgs_manual.lst
```

The idea of the package lists is I can duplicate the environment by performing and
installation of the Ubuntu build host. Then apply all the packages that are marked
as instaled manually. The load_packages script will perform the install. You pass
a file to the command that comtains a package name on each line that you want
installed. The pkgs_manual file output from the "apt-mark showmanual" will load
all the packages that were installed manually.

Note that the apt-mark showmanual output does not include the versions currently installed.
So, running a load_packages file will get you the most recent version.


