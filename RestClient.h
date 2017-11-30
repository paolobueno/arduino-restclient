#include <Arduino.h>
#include "Client.h"

class RestClient {

  public:
    RestClient(const char* host, Client& client);
    RestClient(const char* _host, int _port, Client& client);

    //Generic HTTP Request
    int request(const char* method, const char* path,
                const char* body, char* response);

    // Set a Request Header
    RestClient& setHeader(const char*);
    // Set Content-Type Header
    RestClient& setContentType(const char*);

    RestClient& setClient(Client& client);

    // GET path
    int get(const char*);
    // GET path and response
    int get(const char*, char*);

    // POST path and body
    int post(const char* path, const char* body);
    // POST path and body and response
    int post(const char* path, const char* body, char*);

    // PUT path and body
    int put(const char* path, const char* body);
    // PUT path and body and response
    int put(const char* path, const char* body, char*);

    // DELETE path
    int del(const char*);
    // DELETE path and body
    int del(const char*, const char*);
    // DELETE path and response
    int del(const char*, char*);
    // DELETE path and body and response
    int del(const char*, const char*, char*);

  private:
    Client* client;
    int readResponse(char*);
    void write(const char*);
    const char* host;
    int port;
    int num_headers;
    const char* headers[10];
    const char* contentType;
};
