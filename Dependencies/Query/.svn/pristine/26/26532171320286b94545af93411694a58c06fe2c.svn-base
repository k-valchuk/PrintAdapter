#ifndef ANSWER_H
#define ANSWER_H

#include "Query.h"
namespace rest{

enum StatusCode {
    ClientRequestTimeout = -1,
    
    OK = 200,
    
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    NotAcceptable = 406,
    RequestTimeout = 408,
    Conflict = 409,
    Gone = 410,
    AuthenticationTimeout = 419,
    ClientClosedRequest = 499,

    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnavailable = 503,
    GatewayTimeout = 504,
    HTTPVersionNotSupported = 505,
    LoopDetected = 508,
    BandwidthLimitExceeded = 509,
    NotExtended = 510,
    NetworkAuthenticationRequired = 511,
    UnknownError = 520,
    WebServerIsDown = 521,
    ConnectionTimedOut = 522,
    OriginIsUnreachable = 523,
    ATimeoutOccurred = 524,
    SSLHandshakeFailed = 525,
    InvalidSSLCertificate = 526
};

const QMap<StatusCode, QString> errorStrMap{
        //200
        {OK, "Ok"},
        //400
        {BadRequest, "Bad Request"},
        {Unauthorized, "Unauthorized"}, {Forbidden, "Forbidden"}, {NotFound, "Not Found"},{MethodNotAllowed, "Method Not Allowed"},
        {NotAcceptable, "Not Acceptable"}, {RequestTimeout, "Request Timeout"}, {Conflict, "Conflict"}, {Gone, "Gone"},
        {AuthenticationTimeout, "Authentication Timeout"}, {ClientClosedRequest, "Client Closed Request"},
        //500
        {InternalServerError, "Internal Server Error"}, {NotImplemented, "Not Implemented"}, {BadGateway, "Bad Gateway"},
        {ServiceUnavailable, "Service Unavailable"}, {GatewayTimeout, "Gateway Timeout"}, {HTTPVersionNotSupported, "HTTP Version Not Supported"},
        {LoopDetected, "Loop Detected"}, {BandwidthLimitExceeded, "Bandwidth Limit Exceeded"}, {NotExtended, "Not Extended"},
        {NetworkAuthenticationRequired, "Network Authentication Required"}, {UnknownError, "Unknown Error"}, {WebServerIsDown, "Web Server Is Down"},
        {ConnectionTimedOut, "Connection Timed Out"}, {OriginIsUnreachable, "Origin Is Unreachable"}, {ATimeoutOccurred, "A Timeout Occurred"},
        {SSLHandshakeFailed, "SSL Handshake Failed"}, {InvalidSSLCertificate, "Invalid SSL Certificate"},
    
        //other
        {ClientRequestTimeout, "Client Timeout"}
};

class Response
{
    QString m_answerString;
    StatusCode m_code;
public:
    Response();
    
    QString answerString() const;
    StatusCode code() const;
    bool isSuccess() const;
    
    Response& setAnswerString(QString str);
    Response& setCode(StatusCode code);
};




}



#endif // ANSWER_H
