#ifndef STATUSMONITOR_H
#define STATUSMONITOR_H

#include <QObject>
#include <QThread>
#include <QMutexLocker>
#include <QProcess>
#include <QSettings>
#include <QHostInfo>
#include <QTime>

#include <iostream>

#ifdef _DEBUG
    #include <QDebug>
#endif


// 内存信息结构体
struct MemoryInfo {
    unsigned long long total_memory; // GB
    unsigned long long free_memory;  // GB
};

// CPU 信息结构体
struct CPUInfo {
    std::string name;
    unsigned int cores;
    unsigned int threads;
    unsigned int usage; // CPU 使用率（百分比）
};

// GPU 信息结构体
struct GPUInfo {
    std::string name;
    unsigned long total_memory; // MB
    unsigned long free_memory;  // MB
    unsigned int usage;
};



class StatusMonitor : public QObject {
    Q_OBJECT

public:
    explicit StatusMonitor(QObject* parent = nullptr);
    ~StatusMonitor();

    // get system information
    Q_INVOKABLE const  QString localMachineName();
    Q_INVOKABLE const  QString cpuType();
    Q_INVOKABLE const  QString nvidiaGPU();
    Q_INVOKABLE const  QString memory();
    Q_INVOKABLE const  QString osVersion();

    // 平台相关的函数声明
    MemoryInfo get_memory_info();
    CPUInfo get_cpu_info();
    GPUInfo get_gpu_info();

signals:
    void resultReady(const int cpuUsage, const int memUsage, const int nvidiaGpuUsage);

public slots:
    void monitorProcess();
    void endMonitorProcess();

private:
    QString m_localMachineName;
    unsigned long long m_totalMem;

    QString m_cpuDescribe;
    QString m_memDescribe;
    QString m_osDescribe;


    MemoryInfo mem_m;
    CPUInfo cpu_m;
    GPUInfo gpu_m;

    bool m_monitor_running;

    QMutex mutex;

};

#endif // STATUSMONITOR_H
