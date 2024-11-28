#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/*
 * Global variable
 */
sem_t log_mutex;// mutex for log file

/*
 * Struct for thread arguments
 */
struct arg_t //struct for thread arguments
{
    int connfd;
    struct sockaddr_in clientaddr;
};

/*
 * Function prototypes
 */
void *thread(void *vargp);
void doit(int connfd, struct sockaddr_in *clientaddr);
int sendRequest(int clientfd, rio_t *conn_rio, char *req_header, size_t length, char *method);
size_t forwardResponse(int connfd, rio_t *client_rio);
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n);
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen);
int Rio_writen_w(int fd, void *usrbuf, size_t n);

/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    int listenfd;
    socklen_t clientlen;
    pthread_t tid;
    /* Check arguments */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_listenfd(argv[1]);
    clientlen = sizeof(struct sockaddr_in);
    struct arg_t *thread_arg;
    Sem_init(&log_mutex, 0, 1); // Initialize log mutex
    Signal(SIGPIPE, SIG_IGN);   // ignore SIGPIPE

    while (1)
    {
        thread_arg = Malloc(sizeof(struct arg_t));
        thread_arg->connfd = Accept(listenfd, (SA *)&(thread_arg->clientaddr), &clientlen); // Accept connection
        Pthread_create(&tid, NULL, &thread, thread_arg);                                    // Create a new thread
    }

    Close(listenfd);
    exit(1);
}

/*
 * thread - every thread will detach itself and call doit function to manage proxy send and receive
 */
void *thread(void *vargp)
{
    Pthread_detach(Pthread_self()); // Detach thread to avoid memory leak

    struct arg_t *arg_self = (struct arg_t *)vargp;
    doit(arg_self->connfd, &(arg_self->clientaddr)); // Manage proxy send and receive

    Close(arg_self->connfd); // Close connection
    Free(arg_self);          // Free memory of thread arguments
    return NULL;
}

/*
 * doit - receive request from client and forward the response from server to client
 */
void doit(int connfd, struct sockaddr_in *clientaddr)
{
    char buf[MAXLINE], req_header[MAXLINE]; 
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], pathname[MAXLINE], port[MAXLINE];
    int clientfd;
    rio_t conn_rio, client_rio;
    size_t byte_size = 0, content_length = 0;

    Rio_readinitb(&conn_rio, connfd); // connect to client
    if (!Rio_readlineb_w(&conn_rio, buf, MAXLINE))
    {
        fprintf(stderr, "error: read empty request line\n");
        return;
    }
    if (sscanf(buf, "%s %s %s", method, uri, version) < 3)
    {
        fprintf(stderr, "error: mismatched parameters\n");
        return;
    }
    if (parse_uri(uri, hostname, pathname, port) != 0)
    {
        fprintf(stderr, "error: parse uri error\n");
        return;
    }
    // set request header
    sprintf(req_header, "%s /%s %s\r\n", method, pathname, version);
    size_t n = Rio_readlineb_w(&conn_rio, buf, MAXLINE);
    char header[MAXLINE];
    while (n != 0) // read request header
    {
        if (strncasecmp(buf, "Content-Length", 14) == 0)
        {
            sscanf(buf + 15, "%zu", &content_length);
        }
        strcpy(header, req_header);
        sprintf(req_header, "%s%s", header, buf);

        if (strncmp(buf, "\r\n", 2) == 0)
            break;

        n = Rio_readlineb_w(&conn_rio, buf, MAXLINE);
    }
    if (n == 0)
        return;
    clientfd = open_clientfd(hostname, port); // open client fd
    if (clientfd < 0)
    {
        fprintf(stderr, "error: open client fd error (hostname: %s, port: %s)\n", hostname, port);
        return;
    }
    Rio_readinitb(&client_rio, clientfd);

    int err = 0;
    char sendbuf[MAXLINE];
    // send request header
    if (Rio_writen_w(clientfd, req_header, strlen(req_header)) == -1)
        err = -1;
    else if (strcasecmp(method, "GET") != 0) // if method is not GET
        for (int i = 0; i < content_length; ++i)
        {
            if (Rio_readnb_w(&conn_rio, sendbuf, 1) == 0)
                err = -1;

            if (Rio_writen_w(clientfd, sendbuf, 1) == -1)
                err = -1;
        }
    if (err == 0)
        byte_size = forwardResponse(connfd, &client_rio);// forward response if no error

    format_log_entry(buf, clientaddr, uri, byte_size); // print log with mutex to protect the log file
    P(&log_mutex);
    printf("%s\n", buf);
    V(&log_mutex);

    Close(clientfd);
}


/*
 * forwardResponse - forward response from server to client
 */
size_t forwardResponse(int connfd, rio_t *client_rio)
{
    char buf[MAXLINE];
    size_t byte_size = 0, content_length = 0;

    size_t n = Rio_readlineb_w(client_rio, buf, MAXLINE);
    while (n != 0) // read response header
    {
        byte_size += n;
        if (strncasecmp(buf, "Content-Length", 14) == 0)
            sscanf(buf + 15, "%zu", &content_length);

        if (Rio_writen_w(connfd, buf, strlen(buf)) == -1)
            return 0;

        if (strncmp(buf, "\r\n", 2) == 0)
            break;

        n = Rio_readlineb_w(client_rio, buf, MAXLINE);
    }
    if (n == 0)
        return 0;

    for (int i = 0; i < content_length; ++i) // read response content
    {
        if (Rio_readnb_w(client_rio, buf, 1) == 0)
            return 0;
        if (Rio_writen_w(connfd, buf, 1) == -1)
            return 0;
        ++byte_size;
    }

    return byte_size;
}

/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0)
    {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':')
    {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    }
    else
    {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL)
    {
        pathname[0] = '\0';
    }
    else
    {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    char host[INET_ADDRSTRLEN];

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    if (inet_ntop(AF_INET, &sockaddr->sin_addr, host, sizeof(host)) == NULL)
        unix_error("Convert sockaddr_in to string representation failed\n");

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %s %s %zu", time_str, host, uri, size);
}

/*
 * rewritten rio wrappers,including Rio_readnb_w, Rio_readlineb_w, Rio_writen_w
 */
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n)
{
    ssize_t c;

    if ((c = rio_readnb(rp, usrbuf, n)) < 0)
    {
        fprintf(stderr, "Rio_readnb error\n");
        return 0;
    }
    return c;
}

ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxlen)
{
    ssize_t c;

    if ((c = rio_readlineb(rp, usrbuf, maxlen)) < 0)
    {
        fprintf(stderr, "Rio_readlineb error\n");
        return 0;
    }
    return c;
}

int Rio_writen_w(int fd, void *usrbuf, size_t n)
{
    if (rio_writen(fd, usrbuf, n) != n)
    {
        fprintf(stderr, "Rio_writen error\n");
        return -1;
    }
    return 0;
}