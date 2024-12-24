#include "statusmonitor.h"

StatusMonitor::StatusMonitor(QObject* parent) :
    QObject(parent) {

    m_monitor_running = true;
    m_cpu_usage_thread = 0;
    m_mem_usage_thread = 0;
    m_nvidia_gpu_usage_thread = 0;
}

StatusMonitor::~StatusMonitor() {

}

void StatusMonitor::monitorProcess() {
    while(m_monitor_running) {
        m_mem_usage_thread = getMemUsageThread();
        m_nvidia_gpu_usage_thread = getNvidiaGpuUsageThread();
        m_cpu_usage_thread = getCpuUsgaeThread();

        emit resultReady(m_cpu_usage_thread, m_mem_usage_thread, m_nvidia_gpu_usage_thread);
        if(!m_monitor_running) break;
    }
}


int StatusMonitor::CompareTime(int preTime, int curTime) {
    return curTime - preTime;
}


void StatusMonitor::endMonitorProcess() {
    QMutexLocker locker(&mutex);
    m_monitor_running = !m_monitor_running;
}

const QString StatusMonitor::localMachineName() {
    m_localMachineTime = QHostInfo::localHostName();
    return m_localMachineTime;
}

const QString StatusMonitor::cpuType() {

}
