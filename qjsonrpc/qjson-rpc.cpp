#include <qjsonrpc/qjson-rpc.hpp>

#include <QDateTime>
#include <QJsonArray>
#include <QObject>

#include <cmath>


[[nodiscard]] static bool isInteger(double d)
{
    double const i = std::trunc(d);
    double const fract = d > .0 ? d - i : d + i;
    return qIsNull(fract);
}


namespace rpc {
namespace qjson {

inline namespace _2_0 {

Q_LOGGING_CATEGORY(rpcQJson2_0, "rpc.qjson.2.0")


ParseError parseError(QJsonParseError je)
{
    switch (je.error) {
    case QJsonParseError::ParseError::UnterminatedObject: return ParseError::UnterminatedObject;
    case QJsonParseError::ParseError::MissingNameSeparator: return ParseError::MissingNameSeparator;
    case QJsonParseError::ParseError::UnterminatedArray: return ParseError::UnterminatedArray;
    case QJsonParseError::ParseError::MissingValueSeparator: return ParseError::MissingValueSeparator;
    case QJsonParseError::ParseError::IllegalValue: return ParseError::IllegalValue;
    case QJsonParseError::ParseError::TerminationByNumber: return ParseError::TerminationByNumber;
    case QJsonParseError::ParseError::IllegalNumber: return ParseError::IllegalNumber;
    case QJsonParseError::ParseError::IllegalEscapeSequence: return ParseError::IllegalEscapeSequence;
    case QJsonParseError::ParseError::IllegalUTF8String: return ParseError::IllegalUTF8String;
    case QJsonParseError::ParseError::UnterminatedString: return ParseError::UnterminatedString;
    case QJsonParseError::ParseError::MissingObject: return ParseError::MissingObject;
    case QJsonParseError::ParseError::DeepNesting: return ParseError::DeepNesting;
    case QJsonParseError::ParseError::DocumentTooLarge: return ParseError::DocumentTooLarge;
    case QJsonParseError::ParseError::GarbageAtEnd: return ParseError::GarbageAtEnd;
    default: assert(false); return static_cast<ParseError>(-1);
    }
}


bool JsonRpcObject::isJsonRpcFieldExists(QJsonObject const &jo)
{
    return jo.contains(latin1string::jsonrpc);
}

bool JsonRpcObject::isJsonRpcFieldIsString(QJsonObject const &jo)
{
    return jo.value(latin1string::jsonrpc).isString();
}

JsonRpcObject::JsonRpcObject(QString rpc_version) : QJsonObject({ { latin1string::jsonrpc, qMove(rpc_version) } }) {}

JsonRpcObject::JsonRpcObject(QJsonObject const &o) : QJsonObject(o) {}

JsonRpcObject &JsonRpcObject::operator=(QJsonObject const &o)
{
    *this = JsonRpcObject(o);
    return *this;
}

JsonRpcObject::JsonRpcObject(QJsonObject &&o) noexcept : QJsonObject(qMove(o)) {}

JsonRpcObject &JsonRpcObject::operator=(QJsonObject &&o) noexcept
{
    *this = JsonRpcObject(qMove(o));
    return *this;
}

QString JsonRpcObject::jsonrpc() const
{
    return value(latin1string::jsonrpc).toString();
}

bool JsonRpcObject::isJsonRpcFieldExists() const
{
    return isJsonRpcFieldExists(*this);
}

bool JsonRpcObject::isJsonRpcFieldIsString() const
{
    return isJsonRpcFieldIsString(*this);
}

int JsonRpcObject::checkJsonRpcField() const
{
    if (!isJsonRpcFieldExists() || !isJsonRpcFieldIsString())
        return -1;

    return 0;
}

bool JsonRpcObject::isJsonRpcFieldValid() const
{
    return !checkJsonRpcField();
}

bool JsonRpcObject::isValid() const
{
    if (!isJsonRpcFieldValid())
        return false;

    return true;
}


NotificationObject::NotificationObject(QString method, QJsonValue params) : JsonRpcObject(latin1string::_2_0)
{
    operator[](latin1string::method) = qMove(method);
    switch (params.type()) {
    case QJsonValue::Null:
    case QJsonValue::Bool:
    case QJsonValue::Double:
    case QJsonValue::String: params = QJsonValue(QJsonArray({ params })); break;
    default: break;
    }
    operator[](latin1string::params) = qMove(params);
}

NotificationObject::NotificationObject(JsonRpcObject obj) : JsonRpcObject(qMove(obj)) {}

QString NotificationObject::method() const
{
    return value(latin1string::method).toString();
}

QJsonValue NotificationObject::params() const
{
    return value(latin1string::params);
}

int NotificationObject::checkMethodField() const
{
    QJsonValue const method_val = value(latin1string::method);

    if (!method_val.isString())
        return errorCode(ServerError::NotificationInvalid);

    if (method_val.toString().startsWith(latin1string::rpc_dot))
        return errorCode(ServerError::MethodReserved);

    return 0;
}

int NotificationObject::checkParamsField() const
{
    QJsonValue const params_val = value(latin1string::params);

    if (!params_val.isObject() && !params_val.isArray())
        return errorCode(ServerError::ParametersInvalid);

    return 0;
}

bool NotificationObject::isMethodFieldValid() const
{
    return !checkMethodField();
}

bool NotificationObject::isParamsFieldValid() const
{
    return !checkParamsField();
}

int NotificationObject::checkJsonRpcField() const
{
    int err = JsonRpcObject::checkJsonRpcField();
    if (err)
        return errorCode(ServerError::NotificationInvalid);

    // should contains
    if (value(latin1string::jsonrpc).toString() != latin1string::_2_0)
        return errorCode(ServerError::RpcVersionUnsupported);

    return 0;
}

bool NotificationObject::isValid() const
{
    if (!JsonRpcObject::isValid() || !isMethodFieldValid() ||
        !(size() == 2 || (size() == 3 && contains(latin1string::params) && isParamsFieldValid())))
        return false;

    return true;
}




RequestObject::RequestObject(QString method, QJsonValue id, QJsonValue params)
    : NotificationObject(qMove(method), qMove(params))
{
    if (!id.isString() && !id.isDouble())
        qCDebug(rpcQJson2_0()).noquote() << QObject::tr("bad id type!");
    operator[](latin1string::id) = qMove(id);
}

RequestObject::RequestObject(NotificationObject notification_obj, QJsonValue id)
    : NotificationObject(qMove(notification_obj))
{
    if (!id.isString() && !id.isDouble())
        qCDebug(rpcQJson2_0()).noquote() << QObject::tr("bad id type!");
    operator[](latin1string::id) = qMove(id);
}

RequestObject::RequestObject(JsonRpcObject obj) : NotificationObject(qMove(obj)) {}

QJsonValue RequestObject::id() const
{
    return value(latin1string::id);
}

int RequestObject::checkIdField() const
{
    QJsonValue const id_val = value(latin1string::id);

    if (!id_val.isString() && !id_val.isDouble())
        return errorCode(ServerError::RequestInvalid);

    if (id_val.isDouble()) {
        double const d = id_val.toDouble();
        if (!isInteger(d))
            return errorCode(ServerError::RequestInvalid);
    }

    return 0;
}

bool RequestObject::isIdFieldValid() const
{
    return !checkIdField();
}

int RequestObject::checkMethodField() const
{
    int const err = NotificationObject::checkMethodField();
    if (err == errorCode(ServerError::NotificationInvalid))
        return errorCode(ServerError::RequestInvalid);

    return err;
}

int RequestObject::checkParamsField() const
{
    int const err = NotificationObject::checkParamsField();
    if (err == errorCode(ServerError::NotificationInvalid))
        return errorCode(ServerError::RequestInvalid);

    return err;
}

int RequestObject::checkJsonRpcField() const
{
    int const err = NotificationObject::checkJsonRpcField();
    if (err == errorCode(ServerError::NotificationInvalid))
        return errorCode(ServerError::RequestInvalid);

    return err;
}

bool RequestObject::isValid() const
{
    if (!JsonRpcObject::isValid() || !isMethodFieldValid() ||
        !(size() == 3 || (size() == 4 && contains(latin1string::params) && isParamsFieldValid())))
        return false;

    return true;
}




ErrorObject::ErrorObject(int code, QString message, QJsonValue data)
    : QJsonObject(
          { { latin1string::code, code },
            { latin1string::message, message.isNull() || message.isEmpty() ? errorString(code) : qMove(message) },
            { latin1string::data, qMove(data) } })
{
}

ErrorObject::ErrorObject(QJsonObject const &o) : QJsonObject(o) {}

ErrorObject &ErrorObject::operator=(QJsonObject const &o)
{
    *this = ErrorObject(o);
    return *this;
}

ErrorObject::ErrorObject(QJsonObject &&o) noexcept : QJsonObject(qMove(o)) {}

ErrorObject &ErrorObject::operator=(QJsonObject &&o) noexcept
{
    *this = ErrorObject(qMove(o));
    return *this;
}

int ErrorObject::code() const
{
    return value(latin1string::code).toInt();
}

QString ErrorObject::message() const
{
    return value(latin1string::message).toString();
}

QJsonValue ErrorObject::data() const
{
    return value(latin1string::data);
}

int ErrorObject::checkCodeField() const
{
    QJsonValue const err_val = value(latin1string::code);

    if (!err_val.isDouble())
        return errorCode(ApplicationError::ErrorInvalid);

    double const d = err_val.toDouble();
    if (!isInteger(d))
        return errorCode(ApplicationError::ErrorCodeUndefined);

    return 0;
}

int ErrorObject::checkMessageField() const
{
    QJsonValue const msg_val = value(latin1string::message);

    if (!msg_val.isString())
        return errorCode(ApplicationError::ErrorInvalid);

    if (msg_val.toString().isEmpty())
        return errorCode(ApplicationError::ErrorInvalid);

    return 0;
}

bool ErrorObject::isCodeFieldValid() const
{
    return !checkCodeField();
}

bool ErrorObject::isMessageFieldValid() const
{
    return !checkMessageField();
}

bool ErrorObject::isValid() const
{
    if (!isCodeFieldValid() || !isMessageFieldValid() || (size() != 2 && size() != 3))
        return false;

    return true;
}


ResponseObject::ResponseObject(QJsonValue id, QJsonValue result) : JsonRpcObject(latin1string::_2_0)
{
    if (!id.isString() && !id.isDouble())
        qCDebug(rpcQJson2_0()).noquote() << QObject::tr("bad id type!");
    operator[](latin1string::id) = qMove(id);
    operator[](latin1string::result) = qMove(result);
}

ResponseObject::ResponseObject(ErrorObject error, QJsonValue id) : JsonRpcObject(latin1string::_2_0)
{
    if (!id.isString() && !id.isDouble() && !id.isNull())
        qCDebug(rpcQJson2_0()).noquote() << QObject::tr("bad id type!");
    operator[](latin1string::id) = qMove(id);
    operator[](latin1string::error) = qMove(error);
}

ResponseObject::ResponseObject(JsonRpcObject obj) : JsonRpcObject(qMove(obj)) {}

QJsonValue ResponseObject::id() const
{
    return value(latin1string::id);
}

QJsonValue ResponseObject::result() const
{
    return value(latin1string::result);
}

ErrorObject ResponseObject::error() const
{
    return ErrorObject(value(latin1string::error).toObject());
}

int ResponseObject::checkIdField() const
{
    QJsonValue const id_val = value(latin1string::id);

    if (!id_val.isString() && !id_val.isDouble())
        return errorCode(ApplicationError::ResponseInvalid);

    if (id_val.isDouble()) {
        double const d = id_val.toDouble();
        if (!isInteger(d))
            return errorCode(ApplicationError::ResponseInvalid);
    }

    return 0;
}

int ResponseObject::checkResultField() const
{
    QJsonValue const res_val = value(latin1string::result);

    if (res_val.isNull())
        return errorCode(ApplicationError::ResultInvalid);

    return 0;
}

int ResponseObject::checkErrorField() const
{
    auto const err_obj = ErrorObject(value(latin1string::error).toObject());

    if (int err = err_obj.checkCodeField())
        return err;

    if (int err = err_obj.checkMessageField())
        return err;

    return 0;
}

bool ResponseObject::isIdFieldValid() const
{
    return !checkIdField();
}

bool ResponseObject::isResultFieldValid() const
{
    return !checkResultField();
}

bool ResponseObject::isErrorFieldValid() const
{
    return !checkErrorField();
}

int ResponseObject::checkJsonRpcField() const
{
    int err = JsonRpcObject::checkJsonRpcField();
    if (err)
        return errorCode(ApplicationError::ResponseInvalid);

    // should contains
    if (value(latin1string::jsonrpc).toString() != latin1string::_2_0)
        return errorCode(ApplicationError::RpcVersionUnsupported);

    return 0;
}

bool ResponseObject::isValid() const
{
    if (!JsonRpcObject::isValid() || !isIdFieldValid() || size() != 3 ||
        !((contains(latin1string::result) && isResultFieldValid()) !=
          (contains(latin1string::error) && isErrorFieldValid())))
        return false;

    return true;
}


bool isJsonRpcFieldValid(QJsonObject const &jo)
{
    if (!JsonRpcObject::isJsonRpcFieldExists(jo) || !JsonRpcObject::isJsonRpcFieldIsString(jo))
        return false;

    return true;
}

bool isIdFieldValid(QJsonObject const &jo)
{
    QJsonValue const id_val = jo.value(latin1string::id);

    if (!id_val.isString() && !id_val.isDouble())
        return false;

    if (id_val.isDouble()) {
        double const d = id_val.toDouble();
        if (!isInteger(d))
            return false;
    }

    return true;
}

bool isNotificationMethodFieldValid(QJsonObject const &jo)
{
    QJsonValue const method_val = jo.value(latin1string::method);

    if (!method_val.isString())
        return false;

    if (method_val.toString().startsWith(latin1string::rpc_dot))
        return false;

    return true;
}

bool isNorificationParamsFieldValid(QJsonObject const &jo)
{
    QJsonValue const params_val = jo.value(latin1string::params);

    if (!params_val.isObject() && !params_val.isArray())
        return false;

    return true;
}

bool isRequestIdFieldValid(QJsonObject const &jo)
{
    return isIdFieldValid(jo);
}

bool isRequestMethodFieldValid(QJsonObject const &jo)
{
    return isNotificationMethodFieldValid(jo);
}

bool isRequestParamsFieldValid(QJsonObject const &jo)
{
    return isNorificationParamsFieldValid(jo);
}

bool isResponseIdFieldValid(QJsonObject const &jo)
{
    return isIdFieldValid(jo);
}

bool isResponseResultFieldValid(QJsonObject const &jo)
{
    QJsonValue const res_val = jo.value(latin1string::result);

    if (res_val.isNull())
        return false;

    return true;
}

bool isResponseErrorFieldValid(QJsonObject const &jo)
{
    auto const err_obj = ErrorObject(jo.value(latin1string::error).toObject());

    if ([[maybe_unused]] int err = err_obj.checkCodeField())
        return false;

    if ([[maybe_unused]] int err = err_obj.checkMessageField())
        return false;

    return true;
}


bool isJsonRpcObject(QJsonObject const &jo)
{
    if (!isJsonRpcFieldValid(jo))
        return false;

    if (jo.value(latin1string::jsonrpc).toString() != latin1string::_2_0)
        return false;

    return true;
}

bool isNotificationObject(QJsonObject const &jo)
{
    if (!isJsonRpcObject(jo))
        return false;

    if (!isNotificationMethodFieldValid(jo))
        return false;

    if (jo.size() == 3 && jo.contains(latin1string::params)) {
        if (!isNorificationParamsFieldValid(jo))
            return false;
    } else if (jo.size() != 2)
        return false;

    return true;
}

bool isRequestObject(QJsonObject const &jo)
{
    if (!isJsonRpcObject(jo))
        return false;

    if (!isRequestIdFieldValid(jo))
        return false;

    if (!isRequestMethodFieldValid(jo))
        return false;

    if (jo.size() == 4 && jo.contains(latin1string::params)) {
        if (!isRequestParamsFieldValid(jo))
            return false;
    } else if (jo.size() != 3)
        return false;

    return true;
}

bool isResponseObject(QJsonObject const &jo)
{
    if (!isJsonRpcObject(jo))
        return false;

    if (!isResponseIdFieldValid(jo))
        return false;

    if (jo.size() != 3)
        return false;

    if ((jo.contains(latin1string::result) && isResponseResultFieldValid(jo)) ==
        (jo.contains(latin1string::error) && isResponseErrorFieldValid(jo)))
        return false;

    return true;
}

} // namespace _2_0

} // namespace qjson
} // namespace rpc
