dnl #
dnl # Default ZFS user configuration
dnl #
AC_DEFUN([ZFS_AC_CONFIG_USER], [
	ZFS_AC_CONFIG_USER_SYSTEMD
	ZFS_AC_CONFIG_USER_DRACUT
	ZFS_AC_CONFIG_USER_UDEV
	ZFS_AC_CONFIG_USER_DRACUT
	ZFS_AC_CONFIG_USER_ARCH
	ZFS_AC_CONFIG_USER_IOCTL
	ZFS_AC_CONFIG_USER_ZLIB
	ZFS_AC_CONFIG_USER_LIBUUID
	ZFS_AC_CONFIG_USER_LIBBLKID
	ZFS_AC_CONFIG_USER_FRAME_LARGER_THAN
])
