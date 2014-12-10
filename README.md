# FTP Client in C

This project consists on the development of a download application that implements the FTP protocol, as it is described in RFC959, and adopting the URL syntax as described in RFC1738.

This project was proposed on a Computer Networks course of FEUP (Faculdade de Engenharia da Universidade do Porto).

## How it works

The application's allows you to download a file from an FTP server without much effort. It's like connection to a server via Telnet, but it does (almost) everything for you.

First it establishes the connection with the server using sockets. Then it sends the username and password, changes the directory (if necessary), prints the contents, and then ask you for the file you wish to download and it's done!

## Compiling and running

The application was developed with full IP version independency in mind. Unfortunately that independency made the application misbehave in some networks. To correct that, the address family was changed to AF_INET, that is, IPv4.

If you wish to use both IPv4 and IPv6 or only IPv6, change the hints.ai_family flag from AF_INET to AF_UNSPEC or AF_INET6, respectively.

To compile just execute the following command:

    $ gcc ftpc.c -o ftpc

The to run the application execute the following command:

    $ ./ftpc ftp://<user>:<password>@<host>/<path>
  
You can omit the username and password if you wish anonymous acess to the server. You can also omit the path to acess the root directory.






