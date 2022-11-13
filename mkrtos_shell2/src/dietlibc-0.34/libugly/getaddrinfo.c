#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include "dietfeatures.h"

#ifdef WANT_PLUGPLAY_DNS
extern int __dns_plugplay_interface;
#endif

/* XXX TODO FIXME */

int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) {
  struct addrinfo **tmp;
  int family;
  int error=EAI_NONAME;
  int flags=0;
  tmp=res; *res=0;
  if (hints) {
    if (hints->ai_family && hints->ai_family != PF_INET6 && hints->ai_family != PF_INET) return EAI_FAMILY;
    if (hints->ai_socktype && hints->ai_socktype != SOCK_STREAM && hints->ai_socktype != SOCK_DGRAM) return EAI_SOCKTYPE;
    flags=hints->ai_flags;
    if ((flags&AI_V4MAPPED) && hints->ai_family!=PF_INET6) flags&=~(AI_V4MAPPED|AI_ALL);
  }
  for (family=PF_INET6; ; family=PF_INET) {
    if (!hints || hints->ai_family==family || hints->ai_family==AF_UNSPEC || (flags&(AI_V4MAPPED|AI_ALL))) {
      struct hostent h;
      struct hostent *H;
      int herrno=0;
      char buf[4096];
      int lookupok=0, i;
      char* interface;
      h.h_addr_list=(char**)buf+16;
      h.h_addr_list[1]=0;
      if (node) {
	if ((interface=strchr(node,'%'))) ++interface;
	if (family==PF_INET6 && inet_pton(AF_INET,node,buf)) continue;
	if (inet_pton(family,node,buf)>0) {
	  h.h_name=(char*)node;
	  h.h_addr_list[0]=buf;
	  lookupok=1;
	} else if (!(flags&AI_NUMERICHOST) &&
		   !gethostbyname2_r(node,family,&h,buf,4096,&H,&herrno)) {
	  lookupok=1;
	} else {
	  if (herrno==TRY_AGAIN) { freeaddrinfo(*res); return EAI_AGAIN; }
	}
      } else {
	h.h_name=0;
	h.h_addr_list[0]=buf;
	interface=0;
	memset(buf,0,16);
	if (!(flags&AI_PASSIVE)) {
	  if (family==AF_INET) {
	    buf[0]=127; buf[3]=1;
	  } else
	    buf[15]=1;
	}
	lookupok=1;
      }
      if (lookupok) {

	for (i=0; h.h_addr_list[i]; ++i) {
	  struct ai_v6 {
	    struct addrinfo ai;
	    union {
	      struct sockaddr_in6 ip6;
	      struct sockaddr_in ip4;
	    } ip;
	    char name[1];
	  } *foo;
	  unsigned short port;
	  int len;

	  len=sizeof(struct ai_v6)+(h.h_name?strlen(h.h_name):0);

	  if (!(foo=malloc(len))) goto error;
	  foo->ai.ai_next=0;
	  foo->ai.ai_addrlen=family==PF_INET6?sizeof(struct sockaddr_in6):sizeof(struct sockaddr_in);
	  foo->ai.ai_addr=(struct sockaddr*)&foo->ip;
	  memset(&foo->ip,0,sizeof(foo->ip));
	  foo->ip.ip6.sin6_family=foo->ai.ai_family=family;
	  if (family==PF_INET6) {
	    memmove(&foo->ip.ip6.sin6_addr,h.h_addr_list[i],16);
	    if (interface) foo->ip.ip6.sin6_scope_id=if_nametoindex(interface);
	  } else {
	    /* IPv4 */
	    if (flags&AI_V4MAPPED) {
	      foo->ai.ai_addrlen=sizeof(struct sockaddr_in6);
	      foo->ip.ip6.sin6_addr.s6_addr[10]=foo->ip.ip6.sin6_addr.s6_addr[11]=0xff;
	      memmove(foo->ip.ip6.sin6_addr.s6_addr+12,h.h_addr_list[i],4);
	      foo->ip.ip6.sin6_family=foo->ai.ai_family=PF_INET6;
	    } else
	      memmove(&foo->ip.ip4.sin_addr,h.h_addr_list[i],4);
	  }
#ifdef WANT_PLUGPLAY_DNS
	  if (family==PF_INET6 && node) {
	    int l=strlen(node);
	    if ((l>6 && !strcmp(node+l-6,".local")) || !strchr(node,'.'))
	      foo->ip.ip6.sin6_scope_id=__dns_plugplay_interface;
	  }
#endif
	  if (h.h_name) {
	    foo->ai.ai_canonname=foo->name;
	    memmove(foo->name,h.h_name,strlen(h.h_name)+1);
	  } else
	    foo->ai.ai_canonname=0;

	  for (foo->ai.ai_socktype=SOCK_STREAM; ; foo->ai.ai_socktype=SOCK_DGRAM) {
	    char* type,* x;
	    if (foo->ai.ai_socktype==SOCK_STREAM) {	/* TCP */
	      if (hints && hints->ai_socktype==SOCK_DGRAM) continue;
	      foo->ai.ai_protocol=IPPROTO_TCP;
	      type="tcp";
	    } else {	/* UDP */
	      if (hints && hints->ai_socktype==SOCK_STREAM) break;
	      foo->ai.ai_protocol=IPPROTO_UDP;
	      type="udp";
	    }
	    port=htons(strtol(service?service:"0",&x,0));
	    if (*x) {	/* service is not numeric :-( */
	      struct servent* se;
	      if (!(flags&AI_NUMERICSERV) &&
		  (se=getservbyname(service,type)))
		port=se->s_port;
	      else {
/* can't just fail hard here; maybe the port is defined but not for the protocol we are trying */
		error=EAI_SERVICE;
		if (foo->ai.ai_socktype==SOCK_DGRAM) break;
		continue;
	      }
	    }
	    if (foo->ai.ai_family==PF_INET6)
	      foo->ip.ip6.sin6_port=port;
	    else
	      foo->ip.ip4.sin_port=port;
	    if (!*tmp) *tmp=&(foo->ai); else (*tmp)->ai_next=&(foo->ai);
	    if (!(foo=malloc(len))) goto error;
	    memmove(foo,*tmp,len);
	    tmp=&(*tmp)->ai_next;
	    foo->ai.ai_addr=(struct sockaddr*)&foo->ip;
	    if (foo->ai.ai_canonname)
	      foo->ai.ai_canonname=foo->name;
	    if (foo->ai.ai_socktype==SOCK_DGRAM) break;
	  }
	  free(foo);
	}
      }
    }
    if (hints && hints->ai_family==PF_INET6 && (flags&AI_V4MAPPED) && *res==0)
      flags|=AI_ALL;
    if (family==PF_INET) break;
  }
  if (*res==0) return error; /* kludge kludge... */
  return 0;
error:
  freeaddrinfo(*res);
  return EAI_MEMORY;
}
