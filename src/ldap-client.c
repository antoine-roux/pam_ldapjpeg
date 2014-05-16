__asm__(".symver memcpy,memcpy@GLIBC_2.2.5");
#include "ldap-client.h"

int ldapclient_connect(char* uri, LDAP** ret)
{
	int version = LDAP_VERSION3;
	int reqcert = LDAP_OPT_X_TLS_DEMAND;

        if (ldap_initialize(ret, uri) != LDAP_SUCCESS)
		return LDAP_ERR_INIT;

	if (ldap_set_option(*ret, LDAP_OPT_PROTOCOL_VERSION, &version) &&
	    ldap_set_option(*ret, LDAP_OPT_X_TLS_REQUIRE_CERT, &reqcert)
	    != LDAP_SUCCESS)
		return LDAP_ERR_OPTS;

	if (ldap_start_tls_s(*ret, NULL, NULL) != LDAP_SUCCESS)
		return LDAP_ERR_TLS;

	/* Not binding in LDAP v3 */
	/*if (ldap_simple_bind_s(*ret, NULL, NULL) != LDAP_SUCCESS)
		return LDAP_ERR_BIND;*/

	return RET_OK;
}

int ldapclient_disconnect(LDAP* ldap_connection)
{
	if (ldap_unbind_s(ldap_connection) != LDAP_SUCCESS)
		return -1;

	return RET_OK;
}

int ldap_get_first_binary(LDAP* ldap_connection, char* base,
	char* filter_pattern, char* value, char* field, struct berval*** ret)
{
	char* filter; int filter_length;
	char* attrs[] = { field, NULL };

	LDAPMessage* msg;
	LDAPMessage* entry;

	filter_length = snprintf(NULL, 0, filter_pattern, value) + 1;
	filter = (char*)malloc(filter_length * sizeof(char));
	snprintf(filter, filter_length, filter_pattern, value);

	if (ldap_search_ext_s(ldap_connection, base, LDAP_SCOPE_SUBTREE, filter,
		attrs, 0, NULL, NULL, NULL, LDAP_NO_LIMIT, &msg) != LDAP_SUCCESS) {
		free(filter);
		return LDAP_ERR_SEARCH;
	}

	free(filter);

	if (!(entry = ldap_first_entry(ldap_connection, msg))) {
		ldap_msgfree(msg);
		return LDAP_ERR_NOVALUE;
	}

	if (!(*ret = ldap_get_values_len(ldap_connection, entry, field)))
		return LDAP_ERR_NOVALUE;

	ldap_msgfree(msg);
	return RET_OK;
}

int get_photo(LDAP* ldap_connection, char* user, char** ret, int* len)
{
	int err_code;
	struct berval** list;

	err_code = ldap_get_first_binary(ldap_connection, USER_BASE,
		USER_FILTER, user, USER_PHOTO, &list);
	if (err_code)
		return err_code;

	if (!list[0])
		return LDAP_ERR_NOVALUE;

	*ret = (char*)malloc(list[0]->bv_len);
	memcpy(*ret, list[0]->bv_val, list[0]->bv_len);
	*len = list[0]->bv_len;
	ldap_value_free_len(list);

	return RET_OK;
}
