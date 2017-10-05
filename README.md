# project-1

# README                                                                       #
#                                                                              #
# Description: This file is description for implementation of lisod            #
#                                                                              #
# Authors: Lannie Yulan Miao                              					   #
#          Shanshan Zhang                     	          					   #
#                                                                              #
################################################################################

Description of Files
--------------------------------------------------------------------------------
The server “Lisod” is the select()-based server with support for multiple concurrent clients. It implemented an HTTP 1.1 parser and persistent connections with HEAD, GET, and POST working. 

The server can handle the new connections through adding the description into the descriptor set as well as calling accept() function using the descriptor. 
Also, the server can run through the existing connections looking for data to read. Helper function handle_request() will pass the client requests to generate a corresponding response that can be sent back to the client.

In handle_request() function, the validity of the passing request will be checked(including not implemented method-501, not supported HTTP VERSION-505, malicious request-500). The valid request will be passed to the corresponding helper function that could handle HEAD, GET and POST function. The error message will be returned for the not authorized file(403) and the not found file(404). Otherwise, the response will be generated including the header(HEAD, GET, POST) and the body(GET). For file exceeds the response body size, we reallocate the size of file buffer(File is not be truncated here). 


How To Run
--------------------------------------------------------------------------------
Building and executing the server code should be very simple:
                    make
                    ./lisod 9999 4444 lisod.log dsfsd www
                    curl localhost:9999/images/liso_header.png -X GET -v

./lisod <port> 4444 lisod.log dsfsd www
This command will start the server at dynamic serving port 9999(You can change it ut has to be the same as the connections port in client end). list.log is the log file for server running. www is the directory of the file that client need to query. 

curl localhost:<port>/<directory>/<filename> -X <HTTP method> -v
The method GET can be replaced by HEAD or POST based on needs. 


Http Response Sample Format
--------------------------------------------------------------------------------
HTTP/1.1 501 Not Implemented
Server: liso/1.0
Date: Thu, 05 Oct 2017 01:35:08 GMT
Connection: close
Content-Type: text/html
 
<head>
<title>Error response</title>
</head>
<body>
<h1>Error response</h1>
<p>Error code 501.</p >
<p>Message: Unsupported method ('HET').</p >
<p>Error code explanation: 501 = Server does not support this operation.</p >
