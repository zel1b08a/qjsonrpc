#pragma once

#include <qjsonrpc/qjsonrpc-export.hpp>

#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QString>

#include <limits>

namespace rpc {
namespace qjson {

inline namespace _2_0 {

Q_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(rpcQJson2_0)

namespace string {

constexpr char const *const jsonrpc = "jsonrpc";
constexpr char const *const error_unspecified = "unspecified error";
constexpr char const *const rpc_dot = "rpc.";
constexpr char const *const _2_0 = "2.0";
constexpr char const *const id = "id";
constexpr char const *const method = "method";
constexpr char const *const params = "params";
constexpr char const *const result = "result";
constexpr char const *const error = "error";
constexpr char const *const code = "code";
constexpr char const *const message = "message";
constexpr char const *const data = "data";

} // namespace string

namespace latin1string {

constexpr QLatin1String jsonrpc(string::jsonrpc, 7);
constexpr QLatin1String error_unspecified(string::error_unspecified, 17);
constexpr QLatin1String rpc_dot(string::rpc_dot, 4);
constexpr QLatin1String _2_0(string::_2_0, 3);
constexpr QLatin1String id(string::id, 2);
constexpr QLatin1String method(string::method, 6);
constexpr QLatin1String params(string::params, 6);
constexpr QLatin1String result(string::result, 6);
constexpr QLatin1String error(string::error, 5);
constexpr QLatin1String code(string::code, 4);
constexpr QLatin1String message(string::message, 7);
constexpr QLatin1String data(string::data, 4);

} // namespace latin1string


enum LIBQJSONRPC_EXPORT ErrorType : int {
    ApplicationExtended,
    ServerExtended,
    Transport,
    System,
    Application,
    Server,
    Parse
};
Q_ENUM_NS(ErrorType)
constexpr int error_type_amount = ErrorType::Parse + 1;

constexpr char const *error_type_string[ error_type_amount ] = {
    "application-extended", "server-extended", "transport", "system", "application", "server", "parse"
};

// [-32768...-32000] - for predefined errors
// others negative values is for applications implementation errors
constexpr int const error_type_offset[ error_type_amount ] = {
    1,      // -> 31999 ApplicationExtended
    32'000, // -> 32299 ServerExtended
    32'300, // -> 32399 Transport
    32'400, // -> 32499 System
    32'500, // -> 32599 Application
    32'600, // -> 32699 Server
    32'700  // -> 32768 Parse
};

constexpr int const error_type_size[ error_type_amount ] = {
    error_type_offset[ ServerExtended ] - error_type_offset[ ApplicationExtended ],
    error_type_offset[ Transport ] - error_type_offset[ ServerExtended ],
    error_type_offset[ System ] - error_type_offset[ Transport ],
    error_type_offset[ Application ] - error_type_offset[ System ],
    error_type_offset[ Server ] - error_type_offset[ Application ],
    error_type_offset[ Parse ] - error_type_offset[ Server ],
    -static_cast<int>(std::numeric_limits<short>::min()) - error_type_offset[ Parse ]
};


enum class LIBQJSONRPC_EXPORT TransportError : int {};
Q_ENUM_NS(TransportError)

constexpr char const *transport_error_string[ error_type_size[ Transport ] ] = {};


enum class LIBQJSONRPC_EXPORT SystemError : int {};
Q_ENUM_NS(SystemError)

constexpr char const *system_error_string[ error_type_size[ System ] ] = {};


enum class LIBQJSONRPC_EXPORT ApplicationError : int {
    ResponseInvalid,
    RpcVersionUnsupported,
    ResultInvalid,
    ErrorInvalid,
    ErrorCodeUndefined,
    ErrorMessageIsEmpty,
    ErrorDataInvalid,
    Internal
};
Q_ENUM_NS(ApplicationError)

constexpr char const *application_error_string[ error_type_size[ Application ] ] = {
    "response object is invalid",  "unsupported rpc protocol version",
    "result field is invalid",     "error object is invalid",
    "undefined error code",        "empty error message",
    "error data field is invalid", "internal application error"
};


enum class LIBQJSONRPC_EXPORT ServerError : int {
    RequestInvalid,
    NotificationInvalid,
    RpcVersionUnsupported,
    MethodReserved,
    MethodNotFound,
    ParametersInvalid,
    Internal
};
Q_ENUM_NS(ServerError)

constexpr char const *server_error_string[ error_type_size[ Server ] ] = { "request object is invalid",
                                                                           "notification object is invalid",
                                                                           "unsupported rpc protocol version",
                                                                           "method field is invalid (starts with rpc.)",
                                                                           "method was not found",
                                                                           "method parameters are invalid",
                                                                           "internal server error" };


enum class LIBQJSONRPC_EXPORT ParseError : int {
    UnterminatedObject,
    MissingNameSeparator,
    UnterminatedArray,
    MissingValueSeparator,
    IllegalValue,
    TerminationByNumber,
    IllegalNumber,
    IllegalEscapeSequence,
    IllegalUTF8String,
    UnterminatedString,
    MissingObject,
    DeepNesting,
    DocumentTooLarge,
    GarbageAtEnd
};
Q_ENUM_NS(ParseError)

constexpr char const *parse_error_string[ error_type_size[ Parse ] ] = {
    "unterminated object", "missing name separator", "unterminated array", "missing value separator",
    "illegal value",       "termination by number",  "illegal number",     "illegal escape sequence",
    "illegal utf8 string", "unterminated string",    "missing object",     "deep nesting",
    "document too large",  "garbage at end"
};

[[nodiscard]] ParseError parseError(QJsonParseError je);


[[nodiscard]] inline constexpr int errorCode(TransportError e)
{
    return -error_type_offset[ Transport ] - static_cast<int>(e);
};

[[nodiscard]] inline constexpr int errorCode(SystemError e)
{
    return -error_type_offset[ System ] - static_cast<int>(e);
};

[[nodiscard]] inline constexpr int errorCode(ApplicationError e)
{
    return -error_type_offset[ Application ] - static_cast<int>(e);
};

[[nodiscard]] inline constexpr int errorCode(ServerError e)
{
    return -error_type_offset[ Server ] - static_cast<int>(e);
};

[[nodiscard]] inline constexpr int errorCode(ParseError e)
{
    return -error_type_offset[ Parse ] - static_cast<int>(e);
};

[[nodiscard]] inline constexpr int errorCode(QJsonParseError::ParseError e)
{
    Q_ASSERT(e != QJsonParseError::ParseError::NoError);
    return -error_type_offset[ Parse ] - (static_cast<int>(e) - 1);
};

//  NOTE: err should be negative
[[nodiscard]] inline constexpr ErrorType errorType(int code)
{
    Q_ASSERT(code < 0);
    Q_ASSERT(0 < error_type_amount);
    for (auto i = error_type_amount - 1; 0 <= i; i--) {
        auto const type = static_cast<ErrorType>(i);
        if (error_type_offset[ type ] <= -code)
            return type;
    }
    return static_cast<ErrorType>(-1);
};

[[nodiscard]] inline constexpr char const *const &errorString(int code)
{
    Q_ASSERT(code < 0);
    switch (int err_id = -1; errorType(code)) {
    case Transport:
        err_id = -code - error_type_offset[ Transport ];
        if (error_type_size[ Transport ] <= err_id)
            return string::error_unspecified;
        return transport_error_string[ err_id ];
    case System:
        err_id = -code - error_type_offset[ System ];
        if (error_type_size[ System ] <= err_id)
            return string::error_unspecified;
        return system_error_string[ err_id ];
    case Application:
        err_id = -code - error_type_offset[ Application ];
        if (error_type_size[ Application ] <= err_id)
            return string::error_unspecified;
        return application_error_string[ err_id ];
    case Server:
        err_id = -code - error_type_offset[ Server ];
        if (error_type_size[ Server ] <= err_id)
            return string::error_unspecified;
        return server_error_string[ err_id ];
    case Parse:
        err_id = -code - error_type_offset[ Parse ];
        if (error_type_size[ Parse ] <= err_id)
            return string::error_unspecified;
        return parse_error_string[ err_id ];
    default: return string::error_unspecified;
    }
};


class LIBQJSONRPC_EXPORT JsonRpcObject : public QJsonObject
{
public:
    [[nodiscard]] static bool isJsonRpcFieldExists(QJsonObject const &jo);
    [[nodiscard]] static bool isJsonRpcFieldIsString(QJsonObject const &jo);

public:
    virtual ~JsonRpcObject() = default;
    JsonRpcObject(QString rpc_version);

    JsonRpcObject(JsonRpcObject const &o) = default;
    JsonRpcObject &operator=(JsonRpcObject const &o) = default;

    JsonRpcObject(JsonRpcObject &&o) noexcept = default;
    JsonRpcObject &operator=(JsonRpcObject &&o) noexcept = default;

    JsonRpcObject(QJsonObject const &o);
    JsonRpcObject &operator=(QJsonObject const &o);

    JsonRpcObject(QJsonObject &&o) noexcept;
    JsonRpcObject &operator=(QJsonObject &&o) noexcept;

    [[nodiscard]] QString jsonrpc() const;

    [[nodiscard]] virtual bool isJsonRpcFieldExists() const;
    [[nodiscard]] virtual bool isJsonRpcFieldIsString() const;

    [[nodiscard]] virtual int checkJsonRpcField() const;

    [[nodiscard]] virtual bool isJsonRpcFieldValid() const;

    [[nodiscard]] virtual bool isValid() const;
};


/*
 * not/req from client to server
 * resp/err from server to client
 **/

class LIBQJSONRPC_EXPORT NotificationObject : public JsonRpcObject
{
public:
    NotificationObject(QString method, QJsonValue params = QJsonValue());
    NotificationObject(JsonRpcObject obj);

    [[nodiscard]] QString method() const;
    [[nodiscard]] QJsonValue params() const;

    [[nodiscard]] virtual int checkMethodField() const;
    [[nodiscard]] virtual int checkParamsField() const;

    [[nodiscard]] virtual bool isMethodFieldValid() const;
    [[nodiscard]] virtual bool isParamsFieldValid() const;

    // JsonRpcObject interface
    [[nodiscard]] virtual int checkJsonRpcField() const override;

    [[nodiscard]] virtual bool isValid() const override;
};


class LIBQJSONRPC_EXPORT RequestObject : public NotificationObject
{
public:
    RequestObject(QString method, QJsonValue id, QJsonValue params = QJsonValue());
    RequestObject(NotificationObject notification_obj, QJsonValue id);
    RequestObject(JsonRpcObject obj);

    [[nodiscard]] QJsonValue id() const;

    [[nodiscard]] virtual int checkIdField() const;

    [[nodiscard]] virtual bool isIdFieldValid() const;

    // NotificationObject interface
    [[nodiscard]] virtual int checkMethodField() const override;
    [[nodiscard]] virtual int checkParamsField() const override;

    // JsonRpcObject interface
    [[nodiscard]] virtual int checkJsonRpcField() const override;

    [[nodiscard]] virtual bool isValid() const override;
};


class LIBQJSONRPC_EXPORT ErrorObject : public QJsonObject
{
public:
    virtual ~ErrorObject() = default;
    ErrorObject(int code, QString message = QLatin1String(""), QJsonValue data = QJsonValue());

    ErrorObject(QJsonObject const &o);
    ErrorObject &operator=(QJsonObject const &o);

    ErrorObject(QJsonObject &&o) noexcept;
    ErrorObject &operator=(QJsonObject &&o) noexcept;

    [[nodiscard]] int code() const;
    [[nodiscard]] QString message() const;
    [[nodiscard]] QJsonValue data() const;

    [[nodiscard]] virtual int checkCodeField() const;
    [[nodiscard]] virtual int checkMessageField() const;

    [[nodiscard]] virtual bool isCodeFieldValid() const;
    [[nodiscard]] virtual bool isMessageFieldValid() const;

    [[nodiscard]] virtual bool isValid() const;
};


class LIBQJSONRPC_EXPORT ResponseObject : public JsonRpcObject
{
public:
    ResponseObject(QJsonValue id, QJsonValue result);
    ResponseObject(ErrorObject error, QJsonValue id = QJsonValue());
    ResponseObject(JsonRpcObject obj);

    [[nodiscard]] QJsonValue id() const;
    [[nodiscard]] QJsonValue result() const;
    [[nodiscard]] ErrorObject error() const;

    [[nodiscard]] virtual int checkIdField() const;
    [[nodiscard]] virtual int checkResultField() const;
    [[nodiscard]] virtual int checkErrorField() const;

    [[nodiscard]] virtual bool isIdFieldValid() const;
    [[nodiscard]] virtual bool isResultFieldValid() const;
    [[nodiscard]] virtual bool isErrorFieldValid() const;

    // JsonRpcObject interface
    [[nodiscard]] virtual int checkJsonRpcField() const override;

    [[nodiscard]] virtual bool isValid() const override;
};


// NOTE: if some field must not exists in valid state, you should check existens separatly
[[nodiscard]] LIBQJSONRPC_EXPORT bool isJsonRpcFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isIdFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isNotificationMethodFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isNorificationParamsFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isRequestIdFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isRequestMethodFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isRequestParamsFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isResponseIdFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isResponseResultFieldValid(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isResponseErrorFieldValid(QJsonObject const &jo);


[[nodiscard]] LIBQJSONRPC_EXPORT bool isJsonRpcObject(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isNotificationObject(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isRequestObject(QJsonObject const &jo);
[[nodiscard]] LIBQJSONRPC_EXPORT bool isResponseObject(QJsonObject const &jo);


} // namespace _2_0

} // namespace qjson
} // namespace rpc
