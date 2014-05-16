#include "pam-module.h"
#include "ldap-client.h"
#include "dbus-client.h"

static void logger(pam_handle_t *pamh, int level, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	pam_vsyslog(pamh, LOG_AUTHPRIV | level, fmt, ap);
	va_end(ap);
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	int ruid_orig, euid_orig, suid_orig;

	LDAP* ldap_connection;
	DBusConnection* dbus_connection;

	const char* user;
	char* photo; int photo_size;
	char* icon_path; int icon_path_length;
	FILE *file;

	// Sauvegarde des UID originaux
	getresuid(&ruid_orig, &euid_orig, &suid_orig);
	logger(pamh, LOG_NOTICE, "Module lancé avec ruid=%d, euid=%d, suid=%d", 
		ruid_orig, euid_orig, suid_orig);

	// Le module PAM est lancé avec un UID != EUID, LDAP n'apprecie pas ce
	// paramétrage et le changer à la volée.

	// Copie de l'UID effectif vers l'UID réél
	if (setuid(euid_orig) != 0) {
		logger(pamh, LOG_ERR, "Impossible de copier l'EUID vers l'UID");
		return PAM_CRED_INSUFFICIENT;
	}

	// Récupération du nom de l'utilisateur
	if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS) {
		logger(pamh, LOG_ERR, "Impossible de récuperer le nom de l'utilisateur");
		goto abort;
	}

	logger(pamh, LOG_NOTICE, "Utilisateur à traiter : %s", user);

	if (ldapclient_connect(LDAP_SERVER, &ldap_connection)) {
		logger(pamh, LOG_ERR, "Impossible de se connecter au serveur LDAP");
		goto abort;
	}

	if (get_photo(ldap_connection, (char*)user, &photo, &photo_size)) {
		logger(pamh, LOG_ERR, "Impossible de récuperer la photo de %s", user);
		goto abort;
	}

	ldapclient_disconnect(ldap_connection);

	logger(pamh, LOG_NOTICE, "Photo en mémoire (size=%d)", photo_size);

	// Chemin de la nouvelle photo
	icon_path_length = snprintf(NULL, 0, ICON_PATH, (char*)user) + 1;
	icon_path = (char*)malloc(icon_path_length * sizeof(char));
	snprintf(icon_path, icon_path_length, ICON_PATH, (char*)user);

	file = fopen(icon_path, "wb");
	if (!file) {
		logger(pamh, LOG_ERR, "Photo NON enregistrée. Impossible d'ouvrir %s en écriture",
			icon_path);
		goto abort;
	}
	fwrite(photo, sizeof(char), photo_size, file);
	fclose(file);
	free(photo);

	logger(pamh, LOG_NOTICE, "Photo enregistrée sous %s", icon_path);

	if (dbus_connect(&dbus_connection)) {
		logger(pamh, LOG_ERR, "Impossible de de connecter à DBUS");
		goto abort;
	}

	change_icon(dbus_connection, (char*)user, icon_path);
	logger(pamh, LOG_NOTICE, "Photo définie pour %s", user);
	free(icon_path);

	// Restauration des UID d'origine
	setresuid(ruid_orig, euid_orig, suid_orig);
	return PAM_SUCCESS;

abort:
	// Restauration des UID d'origine
	setresuid(ruid_orig, euid_orig, suid_orig);
	return PAM_ABORT;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char *argv[]) {
	return PAM_SUCCESS;
}

#ifdef PAM_MODULE_ENTRY
PAM_MODULE_ENTRY("pam_ldapjpeg");
#endif
