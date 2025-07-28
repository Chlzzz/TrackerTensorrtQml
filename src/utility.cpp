#include "utility.h"

Utility::Utility(QObject* parent) : QObject(parent) {
    m_engine_init_status = false;
}

Utility::~Utility() {

}

bool Utility::getEngineStatus() const {
    return m_engine_init_status;
}

void Utility::setEngineStatus(const bool engineStatus) {
    if(m_engine_init_status != engineStatus) {
        this -> m_engine_init_status = engineStatus;
        emit engineStatusChanged(this -> m_engine_init_status);
    }
}

void Utility::receivedEngineStatusCorrect(QString status) {
    this -> m_engine_init_status = true;
#ifdef _DEBUG
    qDebug() << "Engine status is correct.";
    qDebug() << status;
#endif
}

void Utility::receivedEngineStatusFalse(QString status) {
    this -> m_engine_init_status = false;
#ifdef _DEBUG
    qDebug() << "Engine init failed. The possibly reason may be: ";
    qDebug() << status;
#endif
}

//"camera_type": "USB",
//"cam_device": "0",
//"infer_device": "Nvidia GPU",
//"task_type": "Detection",
//"network_directory": "./"

void Utility::parseJSValue(QJSValue jsValue, const QVariantList& cameraData) {
    QJsonObject jsonObject = jsValue.toVariant().toJsonObject();

    QString inferDevice = jsonObject.value("infer_device").toString();
    QString NetworkDir = jsonObject.value("network_directory").toString();
    QString TaskType = jsonObject.value("task_type").toString();
    QStringList inferPara;
    inferPara.push_back(inferDevice);
    inferPara.push_back(NetworkDir);
    inferPara.push_back(TaskType);

    std::vector<QStringList> camPara;
    camPara.reserve(cameraData.size());
    for (const QVariant &item : cameraData) {
        QVariantMap cameraInfo = item.toMap();
        QStringList caminfo;
        caminfo.append(cameraInfo["type"].toString());
        caminfo.append(cameraInfo["cameraId"].toString());
        camPara.push_back(caminfo);
    }

    emit sendToThread(camPara, inferPara);

#ifdef _DEBUG
// uncomment the following debug to match the result if something went sideway
    qDebug() << "infer device is: " << inferDevice;
    qDebug() << "NetworkDir is: " << NetworkDir;
    qDebug() << "TaskType is: " << TaskType;
//    qDebug() << "camPara is: " << camPara[0];
// In addition, in debug mode, a debug json file is generated.
    QString headerInfo = QString("// This json file is automatically generated for debug.\n%1 %2")
            .arg("// Generated time:")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n");
    QString jsonString = QJsonDocument(jsonObject).toJson();
    qDebug() << "Received JSON string: " << jsonString;
    qDebug() << "Write JSON file to directory...";
    QFile *file = new QFile(QDir::currentPath() + "/conf/settings_debug.json");
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    if(!file->isOpen()){
        qDebug() << "QFile current path: " << QDir::currentPath() + "/conf/settings_debug.json" ;
        qDebug() << "Error: cannot open json file...";
    }
    file->write(headerInfo.toUtf8() + jsonString.toUtf8());
    file->close();
#endif

}


void Utility::handleRuntimeError(const QString error) {
    this -> m_engine_init_status = false;
    setErrorMessage(error);
    qDebug() << "In function " << __FUNCTION__ << ", error is: " << error;
}

QString Utility::getErrorMessage() const {
    return this -> m_error;
}

void Utility::setErrorMessage(const QString errorMessage) {
    this -> m_error = errorMessage;
    emit errorMessageChanged(this -> m_error);
    qDebug() << "In function " << __FUNCTION__ << ", this -> m_error is: "
             << this -> m_error << ", errorMessage is " << errorMessage;
}

void Utility::handleRuntimeSuccess() {
    this -> m_engine_init_status = true;
    setEngineStatus(this -> m_engine_init_status);
}




























