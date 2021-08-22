#include <Kernel/PerformanceMonitoring.h>
#include <Kernel/Arch/x86/MSR.h>
#include <Kernel/Arch/x86/CPUID.h>
#include <Kernel/Arch/x86/Processor.h>
#include <Kernel/Arch/x86/ProcessorInfo.h>
#include <AK/Format.h>

namespace Kernel {

void PerformanceMonitoring::initialize()
{
    // Check if MSR is available
    if (!MSR::have()) {
        dbgln("PerfMon: MSR is not available on this system");
        return;
    }

    auto& cpuinfo = Processor::current().info();
    dbgln("PerfMon: Family {}, Model {}, Stepping {}", cpuinfo.display_family(), cpuinfo.display_model(), cpuinfo.stepping());

    // Check which performance counters are abailable
    CPUID cpuid(0x80000001);
    auto feature_flags = cpuid.ecx();
    dbgln("PerfMon: CPUID.8000_0001.ECX = {:032b}", feature_flags);
    if (feature_flags & (1 << 23))
        dbgln("PerfMon: Core performance counters available");

    if (feature_flags & (1 << 24))
        dbgln("PerfMon: Northbridge performance counters available");

    if (feature_flags & (1 << 28))
        dbgln("PerfMon: L3 cache performance counters available");



    union CorePerformanceEventSelectRegister {
        struct {
            u8 event_select_lo : 8;
            u8 unit_mask : 8;
            u8 usr : 1;
            u8 os : 1;
            u8 edge : 1;
            u8 reserved1 : 1;
            u8 interrupt: 1;
            u8 reserved2 : 1;
            u8 enabled : 1;
            u8 invert : 1;
            u8 cnt_mask : 8;
            u8 event_select_hi : 4;
            u8 reserved3 : 4;
            u8 guest : 1;
            u8 host : 1;
            u32 reserved4 : 22;
        };
        u64 int_value;
    };


    CorePerformanceEventSelectRegister cache_reads {};
    cache_reads.event_select_lo = 0x60;
    cache_reads.event_select_hi = 0x0;
    cache_reads.unit_mask = 1 << 7;
    cache_reads.usr = 1;
    cache_reads.enabled = 1;
    
    MSR(0xc0010200).set(cache_reads.int_value);

    CorePerformanceEventSelectRegister cache_hits {};
    cache_hits.event_select_lo = 0x64;
    cache_hits.event_select_hi = 0x0;
    cache_hits.unit_mask = 1 << 6;
    cache_hits.usr = 1;
    cache_hits.enabled = 1;
    
    MSR(0xc0010202).set(cache_hits.int_value);

    MSR(0xc0010201).set(0);
    MSR(0xc0010203).set(0);

    MSR perf_event_cache_reads(0xc0010201);
    MSR perf_event_cache_hits(0xc0010203);
    dbgln("PerfMon: cache hit rate so far {}%", perf_event_cache_reads.get() > 0 ? perf_event_cache_hits.get() * 100 / perf_event_cache_reads.get() : 100);

    *(int*)0 = 4;
}

}
