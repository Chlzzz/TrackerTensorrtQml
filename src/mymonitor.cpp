#include <iostream>


// 内存信息结构体
struct MemoryInfo {
    unsigned long total_memory; // MB
    unsigned long free_memory;  // MB
};

// CPU 信息结构体
struct CPUInfo {
    std::string name;
    unsigned int cores;
    unsigned int threads;
    double usage; // CPU 使用率（百分比）
};

// GPU 信息结构体
struct GPUInfo {
    std::string name;
    unsigned long total_memory; // MB
    unsigned long free_memory;  // MB
};

// 平台相关的函数声明
MemoryInfo get_memory_info();
CPUInfo get_cpu_info();
GPUInfo get_gpu_info();

// Linux 平台实现
#if defined(BOOST_OS_LINUX)
    #include <fstream>
    #include <sstream>
    #include <unordered_map>
    #include <thread>
    #include <chrono>

    // 获取内存信息
    MemoryInfo get_memory_info() {
        std::ifstream meminfo("/proc/meminfo");
        std::unordered_map<std::string, unsigned long> memory_data;
        std::string line;

        while (std::getline(meminfo, line)) {
            std::istringstream iss(line);
            std::string key;
            unsigned long value;
            iss >> key >> value;
            key.pop_back(); // 去掉末尾的冒号
            memory_data[key] = value;
        }

        return {
            memory_data["MemTotal"] / 1024, // 总内存（MB）
            memory_data["MemFree"] / 1024   // 空闲内存（MB）
        };
    }

    // 获取 CPU 信息
    CPUInfo get_cpu_info() {
        CPUInfo info;

        // 获取 CPU 名称
        std::ifstream cpuinfo("/proc/cpuinfo");
        std::string line;
        while (std::getline(cpuinfo, line)) {
            if (line.find("model name") != std::string::npos) {
                size_t colon_pos = line.find(":");
                if (colon_pos != std::string::npos) {
                    info.name = line.substr(colon_pos + 2); // 去掉冒号和空格
                    break;
                }
            }
        }

        // 获取 CPU 核心数和线程数
        info.cores = std::thread::hardware_concurrency();
        info.threads = info.cores;

        // 获取 CPU 使用率
        auto get_cpu_stats = []() -> std::unordered_map<std::string, unsigned long> {
            std::ifstream stat_file("/proc/stat");
            std::string line;
            std::getline(stat_file, line);
            std::istringstream iss(line);

            std::unordered_map<std::string, unsigned long> stats;
            std::string cpu_label;
            iss >> cpu_label >> stats["user"] >> stats["nice"] >> stats["system"] >> stats["idle"]
                >> stats["iowait"] >> stats["irq"] >> stats["softirq"] >> stats["steal"] >> stats["guest"] >> stats["guest_nice"];
            return stats;
        };

        auto prev_stats = get_cpu_stats();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto curr_stats = get_cpu_stats();

        unsigned long prev_idle = prev_stats["idle"] + prev_stats["iowait"];
        unsigned long curr_idle = curr_stats["idle"] + curr_stats["iowait"];

        unsigned long prev_non_idle = prev_stats["user"] + prev_stats["nice"] + prev_stats["system"] + prev_stats["irq"] + prev_stats["softirq"] + prev_stats["steal"];
        unsigned long curr_non_idle = curr_stats["user"] + curr_stats["nice"] + curr_stats["system"] + curr_stats["irq"] + curr_stats["softirq"] + curr_stats["steal"];

        unsigned long prev_total = prev_idle + prev_non_idle;
        unsigned long curr_total = curr_idle + curr_non_idle;

        unsigned long total_diff = curr_total - prev_total;
        unsigned long idle_diff = curr_idle - prev_idle;

        info.usage = (total_diff - idle_diff) * 100.0 / total_diff;

        return info;
    }

    // 获取 GPU 信息（需要安装 NVIDIA 驱动和 nvidia-smi）
    GPUInfo get_gpu_info() {
        GPUInfo info;
        std::string command = "nvidia-smi --query-gpu=name,memory.total,memory.free --format=csv,noheader,nounits";
        std::string result;

        FILE* pipe = popen(command.c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            pclose(pipe);
        }

        if (!result.empty()) {
            std::istringstream iss(result);
            std::string name;
            unsigned long total_memory, free_memory;
            iss >> name >> total_memory >> free_memory;
            info.name = name;
            info.total_memory = total_memory;
            info.free_memory = free_memory;
        }

        return info;
    }

// Windows 平台实现
#elif defined(BOOST_OS_WINDOWS)
    #include <windows.h>
    #include <intrin.h>
    #include <pdh.h>
    #pragma comment(lib, "pdh.lib")

    // 获取内存信息
    MemoryInfo get_memory_info() {
        MEMORYSTATUSEX memory_status;
        memory_status.dwLength = sizeof(memory_status);

        if (GlobalMemoryStatusEx(&memory_status)) {
            return {
                static_cast<unsigned long>(memory_status.ullTotalPhys / (1024 * 1024)), // 总内存（MB）
                static_cast<unsigned long>(memory_status.ullAvailPhys / (1024 * 1024))  // 空闲内存（MB）
            };
        } else {
            std::cerr << "Failed to retrieve memory information." << std::endl;
            return {0, 0};
        }
    }

    // 获取 CPU 信息
    CPUInfo get_cpu_info() {
        CPUInfo info;

        // 获取 CPU 名称
        char cpu_name[0x40];
        int cpu_info[4] = { -1 };
        __cpuid(cpu_info, 0x80000002);
        memcpy(cpu_name, cpu_info, sizeof(cpu_info));
        __cpuid(cpu_info, 0x80000003);
        memcpy(cpu_name + 16, cpu_info, sizeof(cpu_info));
        __cpuid(cpu_info, 0x80000004);
        memcpy(cpu_name + 32, cpu_info, sizeof(cpu_info));
        cpu_name[48] = '\0';
        info.name = cpu_name;

        // 获取 CPU 核心数和线程数
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        info.cores = sys_info.dwNumberOfProcessors;
        info.threads = info.cores;

        // 获取 CPU 使用率
        PDH_HQUERY query;
        PDH_HCOUNTER counter;
        PdhOpenQuery(nullptr, 0, &query);
        PdhAddCounter(query, "\\Processor(_Total)\\% Processor Time", 0, &counter);
        PdhCollectQueryData(query);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        PdhCollectQueryData(query);
        PDH_FMT_COUNTERVALUE value;
        PdhGetFormattedCounterValue(counter, PDH_FMT_DOUBLE, nullptr, &value);
        PdhCloseQuery(query);
        info.usage = value.doubleValue;

        return info;
    }

    // 获取 GPU 信息（需要安装 NVIDIA 驱动和 nvidia-smi）
    GPUInfo get_gpu_info() {
        GPUInfo info;
        std::string command = "nvidia-smi --query-gpu=name,memory.total,memory.free --format=csv,noheader,nounits";
        std::string result;

        FILE* pipe = _popen(command.c_str(), "r");
        if (pipe) {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
            _pclose(pipe);
        }

        if (!result.empty()) {
            std::istringstream iss(result);
            std::string name;
            unsigned long total_memory, free_memory;
            iss >> name >> total_memory >> free_memory;
            info.name = name;
            info.total_memory = total_memory;
            info.free_memory = free_memory;
        }

        return info;
    }
#endif

// 主函数
int main() {
    // 获取并输出内存信息
    auto memory_info = get_memory_info();
    std::cout << "Memory Info:" << std::endl;
    std::cout << "  Total Memory: " << memory_info.total_memory << " MB" << std::endl;
    std::cout << "  Free Memory: " << memory_info.free_memory << " MB" << std::endl;

    // 获取并输出 CPU 信息
    auto cpu_info = get_cpu_info();
    std::cout << "CPU Info:" << std::endl;
    std::cout << "  Name: " << cpu_info.name << std::endl;
    std::cout << "  Cores: " << cpu_info.cores << std::endl;
    std::cout << "  Threads: " << cpu_info.threads << std::endl;
    std::cout << "  Usage: " << cpu_info.usage << "%" << std::endl;

    // 获取并输出 GPU 信息
    auto gpu_info = get_gpu_info();
    std::cout << "GPU Info:" << std::endl;
    std::cout << "  Name: " << gpu_info.name << std::endl;
    std::cout << "  Total Memory: " << gpu_info.total_memory << " MB" << std::endl;
    std::cout << "  Free Memory: " << gpu_info.free_memory << " MB" << std::endl;

    return 0;
}