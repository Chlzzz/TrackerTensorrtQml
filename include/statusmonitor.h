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


//// 内存信息结构体
//struct MemoryInfo {
//    uint8_t total_memory; // GB
//    uint8_t free_memory;  // GB
//};

//// CPU 信息结构体
//struct CPUInfo {
//    std::string name;
//    unsigned int cores;
//    unsigned int threads;
//    unsigned int usage; // CPU 使用率（百分比）
//};

//// GPU 信息结构体
//struct GPUInfo {
//    std::string name;
//    uint16_t total_memory; // MB
//    uint16_t free_memory;  // MB
//    unsigned int usage;
//};



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
    uint8_t get_memory_usage();
    uint8_t get_cpu_usage();
    uint8_t get_gpu_usage();

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
    QString m_gpuDescribe;

    int m_cpu_usage_thread;
    int m_mem_usage_thread;
    int m_nvidia_gpu_usage_thread;

    bool m_monitor_running;

    QMutex mutex;

};

#endif // STATUSMONITOR_H
