#ifndef __LDAP_CLIENT_H__
#define __LDAP_CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>

#include "errno.h"

#define LDAP_SERVER	"ldap://server.lan"

#define USER_BASE	"cn=users,dc=server,dc=lan"
#define USER_FILTER	"(&(objectClass=inetOrgPerson)(uid=%s))"
#define USER_PHOTO	"jpegPhoto"

int ldapclient_connect		(char* uri,
				 LDAP** ret);

int ldapclient_disconnect	(LDAP* ldap_connection);

int ldap_get_first_binary	(LDAP* ldap_connection,
				 char* base,
				 char* filter_pattern,
				 char* value,
				 char* field,
				 struct berval*** ret);

int get_photo			(LDAP* ldap_connection,
				 char* user,
				 char** ret,
				 int* len);

#endif
