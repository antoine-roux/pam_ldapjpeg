#include "err.h"

int print_error(int err_code)
{
	char* err_msg;
	
	switch (err_code) {
	case LDAP_ERR_INIT:
	case LDAP_ERR_OPTS:
	case LDAP_ERR_TLS:
	case LDAP_ERR_BIND:
		err_msg = "Echec de la connexion à la base LDAP";
		break;
	case LDAP_ERR_SEARCH:
		err_msg = "Echec de la recherche";
		break;
	case LDAP_ERR_NOVALUE:
		err_msg = "Impossible de trouver l'utilisateur";
		break;
	case DBUS_ERR_INIT:
		err_msg = "Impossible de se connecter à DBUS";
		break;
	default:
		err_msg = "Exception non attendue";
	}

	printf("Erreur %i : %s\n", err_code, err_msg);
	return err_code;
}
