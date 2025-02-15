#ifndef STATUSMONITOR_H
#define STATUSMONITOR_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>
#include <QProcess>
#include <QSettings>
#include <QHostInfo>
#include <QTime>

#include <stdio.h>
#include <nmvl.h>



#ifdef _DEBUG
    #include <QDebug>
#endif

class StatusMonitor : public QObject {
    Q_OBJECT

public:
    explicit StatusMonitor(QObject* parent = nullptr);
    ~StatusMonitor();

    // get system information
    Q_INVOKABLE const  QString localMachineName();
    Q_INVOKABLE const  QString cpuType();
    Q_INVOKABLE const  QString intelGPU();
    Q_INVOKABLE const  QString nvidiaGPU();
    Q_INVOKABLE const  QString memory();
    Q_INVOKABLE const  QString osVersion();
    Q_INVOKABLE const  QString intelGpuInfo();

    const int getCpuUsgaeThread();
    const int getMemUsageThread();
    const int getNvidiaGpuUsageThread();

signals:
    void resultReady(const int cpuUsage, const int memUsage, const int nvidiaGpuUsage);

public slots:
    void monitorProcess();
    void endMonitorProcess();

protected:
    size_t CompareTime(size_t preTime, size_t curTime);

private:
    QString m_localMachineTime;
    unsigned long long m_totalMem;

    QString m_cpuDescripe;
    QString m_memDescribe;
    QString m_osDescirbe;

    int m_cpu_usage_thread;
    int m_mem_usage_thread;
    int m_nvidia_gpu_usage_thread;

    bool m_monitor_running;

    QString m_cl_info_path;

    QString m_intel_gpu_opencl_info;

    QMutex mutex;
    QString m_powershell_script_path;

};

#endif // STATUSMONITOR_H
