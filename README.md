# usrsctp for ARC platform
[![Coverity Scan Build Status](https://scan.coverity.com/projects/13430/badge.svg)](https://scan.coverity.com/projects/usrsctp)

This is a userland SCTP stack supporting FreeBSD, Linux, Mac OS X and Windows.


Cross-compilation for Linux systems
In the folder `usrsctp` type

    $ ./bootstrap
    
``Optional``
Create a config.cache with the following ac_cv_* and glib_* entries

    ac_cv_func_posix_getpwuid_r=${ac_cv_func_posix_getpwuid_r=yes}
    ac_cv_func_posix_getgrgid_r=${ac_cv_func_posix_getgrgid_r=yes}
    ac_cv_have_abstract_sockets=${ac_cv_have_abstract_sockets=no}
    glib_cv_long_long_format=${glib_cv_long_long_format=ll}
    glib_cv_stack_grows=${glib_cv_stack_grows=no}
    glib_cv_has__inline=${glib_cv_has__inline=yes}
    glib_cv_has__inline__=${glib_cv_has__inline__=yes}
    glib_cv_uscore=${glib_cv_uscore=no}
    glib_cv_use_pid_surrogate=${glib_cv_use_pid_surrogate=yes}
    
    $ ./configure --cache-file=config.cache --host=arc-linux-uclibc --enable-shared=yes --enable-static=yes --prefix=/opt/arclibs
 Since ARC 700 has no support for __atomic*() or __sync*() variants, I incorporated software support for the same using pthread library  in ./usrsctplib/user_atomic.h file
    $ make

Now, the library `libusrsctp.la` has been built in the subdirectory `usrsctplib`, and the example programs are ready to run from the subdirectory `programs`.

If you have root privileges or are in the sudoer group, you can install the library in `$prefix/lib` and copy the header file to `$prefix/include` with the command

    $ sudo make install


See [manual](Manual.md) for more information.

The status of continuous integration testing is available from [grid](http://212.201.121.110:18010/grid) and [waterfall](http://212.201.121.110:18010/waterfall).
If you are only interested in a single branch, just append `?branch=BRANCHNAME` to the URL, for example [waterfall](http://212.201.121.110:18010/waterfall?branch=master).
