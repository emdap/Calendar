#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "calendar.h"

#ifndef PORT
  #define PORT 54746
#endif

int setup (void) {
  int on = 1;
  struct sockaddr_in self;
  int listenfd;
  if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }

  // Make sure we can reuse the port immediately after the
  // server terminates.
  if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                      &on, sizeof(on)) == -1) {
    perror("setsockopt -- REUSEADDR");
  }

  memset(&self, '\0', sizeof(self));
  self.sin_family = AF_INET;
  self.sin_addr.s_addr = INADDR_ANY;
  self.sin_port = htons(PORT);
  printf("Listening on %d\n", PORT);

  if (bind(listenfd, (struct sockaddr *)&self, sizeof(self)) == -1) {
    perror("bind"); // probably means port is in use
    exit(1);
  }

  if (listen(listenfd, 5) == -1) {
    perror("listen");
    exit(1);
  }
  return listenfd;
}

int main(void) {
  int listenfd;
  int fd, nbytes;
  struct sockaddr_in peer;
  socklen_t socklen;
  char buf[20];
  char *bufptr = malloc(sizeof(char));
  bufptr = NULL;
  const char *end = "\n";
  const char *user = "?";
  
  listenfd = setup();
  while (1) {
    socklen = sizeof(peer);
    if ((fd = accept(listenfd, (struct sockaddr *)&peer, &socklen)) < 0) {
      perror("accept");
    } else {
      printf("New connection on port %d\n", 
             ntohs(peer.sin_port));
      write(fd, "Please enter a username: ",25);
      read(fd, buf, sizeof(buf) - 1);
      
      while ((nbytes = read(fd, buf, sizeof (buf) - 1)) > 0) {
        bufptr = (char*)realloc(bufptr, 20);
        strcat(bufptr, buf);
        printf("%d",nbytes);
        printf ("Next message: **%s**\n", buf);
        if(strcmp(&buf[nbytes-1],user)==0){
        	printf("found");
        	}
        if(strcmp(&buf[nbytes-1],end)==0){
            printf("end: ");
            printf("%s",bufptr);
            free(bufptr);
            bufptr = malloc(sizeof(char));
            }
        printf("^");
        memset(buf, 0, 20);
      }
      close (fd);
    }
  }
}
   
