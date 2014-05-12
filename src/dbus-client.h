#ifndef __DBUS_CLIENT_H__
#define __DBUS_CLIENT_H__

#include <stdio.h>
#include <dbus/dbus.h>

#include "errno.h"

#define DBUS_SERVICE	"org.freedesktop.Accounts"

int dbus_connect	(DBusConnection** ret);

int dbus_call_method	(DBusConnection* dbus_connection,
			 const char* bus_name,
			 const char* path,
			 const char* iface,
			 const char* method,
			 char** params,
			 char** ret);

int dbus_get_user_path	(DBusConnection* dbus_connection,
			 char* user,
			 char** ret);

void change_icon	(DBusConnection* dbus_connection,
			 char* user,
			 char* new_path);

#endif
