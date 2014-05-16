#include "main.h"
#include "ldap-client.h"
#include "dbus-client.h"

int main(int argc, char *argv[])
{
	int err_code;

	LDAP* ldap_connection;
	DBusConnection* dbus_connection;

	char* photo; int photo_size;
	char* icon_path; int icon_path_length;
	FILE *file;

	if (argc != 2) {
		printf("Usage: %s username\n", argv[0]);
		return EXIT_FAILURE;
	}

	printf("Connexion à l'annuaire ... ");
	err_code = ldapclient_connect(LDAP_SERVER, &ldap_connection);
	if (err_code) return print_error(err_code);
	printf("OK\n");

	printf("Récupération de l'image ... ");
	err_code = get_photo(ldap_connection, argv[1], &photo, &photo_size);
	if (err_code) return print_error(err_code);
	ldapclient_disconnect(ldap_connection);
	printf("OK\n");

	printf("Enregistrement de l'image ... ");
	icon_path_length = snprintf(NULL, 0, ICON_PATH, argv[1]) + 1;
	icon_path = (char*)malloc(icon_path_length * sizeof(char));
	snprintf(icon_path, icon_path_length, ICON_PATH, argv[1]);

	file = fopen(icon_path, "wb");
	if (!file) {
		printf("Echec à l'ouverture du fichier\n");
		return EXIT_FAILURE;
	}
	fwrite(photo, sizeof(char), photo_size, file);
	fclose(file);
	free(photo);
	printf("OK\n");

	printf("Connexion à DBUS ... ");
	err_code = dbus_connect(&dbus_connection);
	if (err_code) return print_error(err_code);
	printf("OK\n");

	printf("Changement de l'image ... ");
	change_icon(dbus_connection, argv[1], icon_path);
	printf("OK\n");

	free(icon_path);
	return EXIT_SUCCESS;
}
