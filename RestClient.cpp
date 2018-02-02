#include "RestClient.h"

#define DEFAULT_MIME "application/x-www-form-urlencoded"

#define STATUS_START 0
#define STATUS_CODE 1
#define STATUS_PRE_BODY 2
#define STATUS_BODY 3

RestClient::RestClient(const char* _host, Client& client) {
  host = _host;
  port = 80;
  num_headers = 0;
  setContentType(DEFAULT_MIME);
  setClient(client);
}

RestClient::RestClient(const char* _host, int _port, Client& client) {
  host = _host;
  port = _port;
  num_headers = 0;
  setContentType(DEFAULT_MIME);
  setClient(client);
}

RestClient& RestClient::setClient(Client& client) {
  this->client = &client;
  return *this;
}

// GET path
int RestClient::get(const char* path) {
  return request("GET", path, NULL, NULL, 0);
}

// GET path with response
int RestClient::get(const char* path, char* response, int length) {
  return request("GET", path, NULL, response, length);
}

// POST path and body
int RestClient::post(const char* path, const char* body) {
  return request("POST", path, body, NULL, 0);
}

// POST path and body with response
int RestClient::post(const char* path, const char* body, char* response,
                     int length) {
  return request("POST", path, body, response, length);
}

// PUT path and body
int RestClient::put(const char* path, const char* body) {
  return request("PUT", path, body, NULL, 0);
}

// PUT path and body with response
int RestClient::put(const char* path, const char* body, char* response,
                    int length) {
  return request("PUT", path, body, response, length);
}

// DELETE path
int RestClient::del(const char* path) {
  return request("DELETE", path, NULL, NULL, 0);
}

// DELETE path and response
int RestClient::del(const char* path, char* response, int length) {
  return request("DELETE", path, NULL, response, length);
}

// DELETE path and body
int RestClient::del(const char* path, const char* body) {
  return request("DELETE", path, body, NULL, 0);
}

// DELETE path and body with response
int RestClient::del(const char* path, const char* body, char* response,
                    int length) {
  return request("DELETE", path, body, response, length);
}

void RestClient::write(const char* string) { client->print(string); }

RestClient& RestClient::setHeader(const char* header) {
  headers[num_headers] = header;
  num_headers++;
  return *this;
}

RestClient& RestClient::setContentType(const char* contentTypeValue) {
  contentType = contentTypeValue;
  return *this;
}

// The mother- generic request method.
//
int RestClient::request(const char* method, const char* path, const char* body,
                        char* response, int length) {
  if (!client->connect(host, port)) {
    return 0;
  }
  // Make a HTTP request line:
  write(method);
  write(" ");
  write(path);
  write(" HTTP/1.1\r\n");
  for (int i = 0; i < num_headers; i++) {
    write(headers[i]);
    write("\r\n");
  }
  write("Host: ");
  write(host);
  write("\r\n");
  write("Connection: close\r\n");

  if (body != NULL) {
    char contentLength[30];
    sprintf(contentLength, "Content-Length: %d\r\n", strlen(body));
    write(contentLength);

    write("Content-Type: ");
    write(contentType);
    write("\r\n");
  }

  write("\r\n");

  if (body != NULL) {
    write(body);
    write("\r\n");
    write("\r\n");
  }

  // make sure you write all those bytes.
  client->flush();

  int statusCode = readResponse(response, length);

  // cleanup
  num_headers = 0;
  client->stop();
  delay(50);

  return statusCode;
}

int RestClient::readResponse(char* response, int length) {
  // an http request ends with a blank line
  boolean currentLineIsBlank = true;
  char statusCode[4];
  int statusCodeIdx = 0;
  int writeIdx = 0;
  int code = 0;

  if (response) {
    memset(response, 0, length);
  }

  int status = STATUS_START;
  while (client->connected()) {
    if (client->available()) {
      char c = client->read();
      switch (status) {
        case STATUS_START:
          if (c == ' ') {
            status = STATUS_CODE;
          }
          break;
        case STATUS_CODE:
          if (statusCodeIdx < 3 && c != ' ') {
            statusCode[statusCodeIdx++] = c;
          } else {
            statusCode[statusCodeIdx] = '\0';
            code = atoi(statusCode);
            status = STATUS_PRE_BODY;
          }
          break;
        case STATUS_PRE_BODY:
					// skip one line because of HTTP protocol
          if (c == '\n' && currentLineIsBlank) {
            status = STATUS_BODY;
          } else if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          }
          break;
        case STATUS_BODY:
          if (response && writeIdx < length - 1) {
            response[writeIdx++] = c;
          }
          break;
      }
    }
  }
  return code;
}
