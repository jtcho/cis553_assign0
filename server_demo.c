#include "demo.h"

struct sym_list Head;	/* head of singly-linked list */

/*
 * Daemon provides an interactive associative memory
 * via a socket-based interface. Clients either set
 * values with an assignment statement or access
 * values with a $ preface. When the value is
 * accessed, we write it onto the client's socket.
 * We currently do this as an iterative server for
 * reasons of queuing and serialization. If the
 * server is made concurrent, the database will have
 * to have serialized access and copy control - this
 * is not necessary yet.
 *
 * Program notes:
 * Parsing is done
 * with find_dollar() and find_equals().
 * Storage management is carried out by the insert()
 * and lookup() routines.
 * save() and restore() routines added to
 * use disk storage to maintain memory across
 * invocations.
 * Iterative server code is copied from Stevens, "UNIX Network
 * Programming: Networking APIs: Sockets and XTI," p. 101
 *
 */

int main(int argc, char *argv[], char *env[] ) {
    int server_fd, create_service(), nbytes;
    void service(), save(), restore();
    socklen_t len;
    struct sockaddr_in cliaddr;
    char buf[BUFSIZE];
    extern int close();

    server_fd = create_service();

    while( HELL_NOT_FROZEN ) {
        len = sizeof( cliaddr );
        nbytes = recvfrom(server_fd, buf, BUFSIZE, 0,
                (struct sockaddr*)&cliaddr, &len);
        buf[nbytes] = 0;
        printf("\nReceived the following:\n");
        printf("%s\n", buf);
        
	    //restore( DATABASE );
	    //service(connection_fd);
	    //save( DATABASE );

	    //close( connection_fd );
    }
}

void service( int fd ) {
    char buf[BUFSIZE];
    extern  void fix_tcl(), insert();

    //SERVER LOOP
    for(;;) {
        char *ptr, *name, *value;

        fix_tcl( buf ); /* hack to interface with tcl scripting language */

        /* ASSIGN */
        if( (ptr = find_equals( buf )) != (char *) NULL ) {
            #ifdef EBUG
            fprintf( stderr, "ASSIGN: %s\n", buf );
            dump( buf );
            #endif
            *ptr = EOS;
       	    name = strsave( buf ); 
       	    value = strsave( ++ptr );
       	    insert( name, value );
            //fputs( "\n", client_reply );
       	    //fflush( client_reply );
            #ifdef EBUG
            fprintf( stderr, "REPLY: <>\n" );
            #endif
	    }
        else if ((ptr = find_dollar( buf )) != (char *) NULL) /* RETRIEVE */ {
	        char *reply, *find_newline; 
            #ifdef EBUG
	        dump( ptr );
            #endif
	        /* removes trailing newline if found */ 
	        if( (find_newline = strrchr( ptr, NEWLINE )) != NULL )
	        *find_newline = EOS;

	        if( (reply = lookup( ++ptr )) != NULL ) {
	            //fputs( reply, client_reply );
	            //fflush( client_reply );
                #ifdef EBUG
                fprintf( stderr, "REPLY: <%s>\n", reply );
                #endif
	        }
	        else {
	            //fputs( "\n", client_reply );
	            //fflush( client_reply );
                #ifdef EBUG
                fprintf( stderr, "REPLY: <>\n" );
                #endif
	        }
	    }
      else {
        #ifdef EBUG
        fprintf( stderr, "GARBAGE\n" );
        #endif
	    }
    }
    return;
}

int create_service() {
    int listenfd;
    /*
     * struct sockaddr_in {
     *     _uint8_t         sin_len;
     *     sa_family_t      sin_family;
     *     in_port_t        sin_port;
     *     struct in_addr   sin_addr;
     *     char             sin_zero[8];
     * }
     */
    struct sockaddr_in servaddr;

    /*
     * Create socket and get the file descriptor.
     * For UDP we will use SOCK_DGRAM instead of SOCK_STREAM.
     */
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    if( listenfd < 0 ) {
        perror( "creating socket for listenfd" );
        exit( ERR_SOCKET );
    }

    bzero( &servaddr, sizeof(servaddr) );
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    servaddr.sin_port = htons( CIS553_PORT );

    /*
     * Name the socket by assigning a port to it.
     * n.b. SA macro from defs.h for struct sock_addr
     */
    if( bind( listenfd, (SA *) &servaddr, sizeof(servaddr) ) < 0 ) {
        perror( "bind on listenfd");
        exit( ERR_BIND );
    }

    //if( listen( listenfd, LISTENQ ) < 0 ) {
    //    perror( "listen on listenfd" );
    //    exit( ERR_LISTEN );
    //}

    return( listenfd );
}

void fix_tcl( char *buf ) {
    char *ptr;

    #define CARRIAGE_RETURN '\r'
    if( (ptr = strrchr( buf, CARRIAGE_RETURN )) != NULL )
        *ptr = EOS;
    return;
}
 
void dump( char *buf ) {
  fprintf( stderr, "strlen(buf)=%d, buf=<%s>\n", (int) strlen(buf), buf );
  {
    int i;

    for( i=0; buf[i] != EOS; i++ )
        fprintf( stderr, "%d:%c:%x\n", i, buf[i], buf[i] );
  }
}
