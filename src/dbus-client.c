#include "dbus-client.h"

int dbus_connect(DBusConnection** ret)
{
	DBusError error;
	dbus_error_init(&error);
	*ret = dbus_bus_get(DBUS_BUS_SYSTEM, &error);

	if (!*ret || dbus_error_is_set(&error)) {
		dbus_error_free(&error);
		return DBUS_ERR_INIT;
	}

	dbus_error_free(&error);
	return RET_OK;
}

int dbus_call_method(DBusConnection* dbus_connection, const char* bus_name,
	const char* path, const char* iface, const char* method,
	char** params, char** ret)
{
	DBusMessage *message;
	DBusMessageIter args;
	DBusPendingCall* pending;
	int param_index, param_count = sizeof(params) / sizeof(char*);

	message = dbus_message_new_method_call(
		bus_name,
		path,
		iface,
		method);

	dbus_message_iter_init_append(message, &args);
	for (param_index = 0; param_index < param_count; param_index++) {
		dbus_message_iter_append_basic(
			&args, DBUS_TYPE_STRING,
			&params[param_index]);
	}

	dbus_connection_send_with_reply(dbus_connection, message, &pending, -1);
	dbus_connection_flush(dbus_connection);
	dbus_message_unref(message);

	dbus_pending_call_block(pending);
	if (ret) message = dbus_pending_call_steal_reply(pending);
	dbus_pending_call_unref(pending);

	if (ret) {
		dbus_message_iter_init(message, &args);
		dbus_message_iter_get_basic(&args, ret);
		dbus_message_unref(message);
	}

	return RET_OK;
}

int dbus_get_user_path(DBusConnection* dbus_connection, char* user, char** ret)
{
	char** params = { &user };

	return dbus_call_method(
		dbus_connection,
		DBUS_SERVICE,
		"/org/freedesktop/Accounts",
		"org.freedesktop.Accounts",
		"FindUserByName",
		params, ret);
}

void change_icon(DBusConnection* dbus_connection, char* user, char* new_path)
{
	char* user_path;
	char** params = { &new_path };

	dbus_get_user_path(dbus_connection, user, &user_path);
	dbus_call_method(
		dbus_connection,
		DBUS_SERVICE,
		user_path,
		"org.freedesktop.Accounts.User",
		"SetIconFile",
		params, NULL);
}
