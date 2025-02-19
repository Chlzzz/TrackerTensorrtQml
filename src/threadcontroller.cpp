#include "threadcontroller.h"

ThreadController::ThreadController(QObject* parent, OCVImageProvider* imageProvider,
    StatusMonitor* statusMonitor, Utility* utility) : QObject(parent) {
    
    //imageProcess = new ImageProcess();
    statusMonitor = new StatusMonitor();

    //imageProcess->moveToThread(&readingThread);
    statusMonitor -> moveToThread(&monitorThread);

    connect(this, SIGNAL(startMonitorProcess()), statusMonitor, SLOT(monitorProcess()));
    
    // have to use 'Qt::ConnectionType::DirectConnection', otherwise connect wont work
    // DirectConnection，槽函数立即在发送信号的线程中执行
    connect(this, SIGNAL(monitorThreadExit()), statusMonitor, SLOT(endMonitorProcess()), Qt::ConnectionType::DirectConnection); 
    connect(&monitorThread, QThread::finished, statusMonitor, &QObject::deleteLater);
    connect(statusMonitor, &StatusMonitor::resultReady, this, &ThreadController::handleResult);

    // connect(this, SIGNAL(operateImageThread()), imageProcess, SLOTS(updateImage(QImage)));
    // connect(this, SIGNAL(imageProcessExit()), imageProcess, SLOT(endCapture()), Qt::ConnectionType::DirectConnection);
    // connect(&readingThread, &QThread::finished, imageProcess, &QObject::deleteLater);
    // connect(imageProcess, SIGNAL(sendImage(QImage)), imageProvider, SLOT(updateImage(QImage)));

    // // check the parameters...
    // connect(utility, &Utility::sendToThread, imageProcess, &ImageProcess::checkInferParameter, Qt::ConnectionType::DirectConnection);

    // // start yolo
    // connect(this, &ThreadController::sendYoloStart, imageProcess, &ImageProcess::changeYoloDetectStatus, Qt::ConnectionType::DirectConnection);

    // // TODO: send error message when init engine
    // // sender: imageProcess
    // // signal: sendRuntimeError
    // // receiver: utility
    // // slot: handleError
    // // handleError will write error message
    // // In GUI, just call utility.getEngineStatus() to receive error message
    // connect(imageProcess, &ImageProcess::sendCameraError, utility, &Utility::handleRuntimeError);
    // connect(imageProcess, &ImageProcess::sendInferDeviceError, utility, &Utility::handleRuntimeError);

    // // sender: imageProcess, signal: sendInferDeviceSuccess
    // // receiver: utility, slot: handleRuntimeSuccess
    // connect(imageProcess, &ImageProcess::sendInferDeviceSuccess, utility, &Utility::handleRuntimeSuccess);
    monitorThread.start();
    emit startMonitorProcess();
}


ThreadController::~ThreadController() {
    monitorThread.quit();
    monitorThread.wait();

    readingThread.quit();
    readingThread.wait();
}


void ThreadController::monitorThreadFinished(){
#ifdef _DEBUG
    qDebug() << "Called " << _FUNCTION__ << ", exiting thread... ";
#endif
    emit monitorThreadExit();
}

void ThreadController::imageThreadStart(){
    readingThread.start();
    emit operateImageThread(0);
}

void ThreadController::imageThreadFinished(){
#ifdef _DEBUG
    qDebug() << "Called " << _FUNCTION__ << ", exiting thread... ";
#endif
    emit imageProcessExit();
}

/// @brief 将statusMonitor ready 后得到的cpu,mem,gpu参数设置好
/// @param cpuUsage 
/// @param memUsage 
/// @param nvidiaGpuUsage 
void ThreadController::handleResult(const int cpuUsage, const int memUsage, const int nvidiaGpuUsage){
    setCpuUsage(cpuUsage);
    setMemUsage(memUsage);
    setNvidiaGpuUsage(nvidiaGpuUsage);
}


int ThreadController::getCpuUsage() const {
    return m_cpu_usage;
}

void ThreadController::setCpuUsage(const int cpuUsage) {
    if(m_cpu_usage != cpuUsage) {
        m_cpu_usage = cpuUsage;
        emit cpuUsageChanged(m_cpu_usage);
    }
}


int ThreadController::getMemUsage() const {
    return m_mem_usage;
}

void ThreadController::setMemUsage(const int memUsage) {
    if(m_mem_usage != memUsage) {
        m_mem_usage = memUsage;
        emit memUsageChanged(m_mem_usage);
    }
}


int ThreadController::getNvidiaGpuUsage() const{
    return m_nvidia_gpu_usage;
}

void ThreadController::setNvidiaGpuUsage(const int nvidiaGpuUsage){
    if(nvidiaGpuUsage != m_nvidia_gpu_usage){
        m_nvidia_gpu_usage = nvidiaGpuUsage;
        emit nvidiaGpuUsageChanged(m_nvidia_gpu_usage);
    }
}

// void ThreadController::startYoloDetect() {
//     emit sendYoloStart();
// }
