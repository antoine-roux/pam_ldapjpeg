#ifndef __LDAP_CLIENT_H__
#define __LDAP_CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>

#include "errno.h"

#ifndef LDAP_SERVER
#pragma error "You must define LDAP_SERVER"
//#define LDAP_SERVER	"ldap://server.lan"
#endif

#ifndef USER_BASE
#pragma error "You must define USER_BASE"
//#define USER_BASE	"cn=users,dc=server,dc=lan"
#endif

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
