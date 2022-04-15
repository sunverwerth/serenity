#pragma once

#include <AK/Format.h>
#include <AK/String.h>
#include <LibCore/ElapsedTimer.h>
#include <serenity.h>

#include "Parameter.h"

class Scenario {
public:
    Scenario(StringView name, StringView description)
        : m_name{name}
        , m_description{description}
    {
    }

    virtual ~Scenario() {}

    void add(Parameter& parameter) { m_parameters.append(&parameter); }
    
    void run()
    {
        auto string_id = perf_register_string(m_name.characters(), m_name.length());
        perf_event(PERF_EVENT_SIGNPOST, string_id, 0);

        size_t num_permutations { 1 };
        for (auto const& parameter: m_parameters){
            num_permutations *= parameter->count();
        }

        outln("=== Running scenario '{}' with {} permutations", m_name, num_permutations);
        initialize();
        
        constexpr int num_runs = 100;

        int slowest_run = NumericLimits<int>::min();
        int fastest_run = NumericLimits<int>::max();
        String slowest_permutation;
        String fastest_permutation;

        do {
            out("*\033[1D");
            fflush(stdout);
            initialize_run();
            auto timer = Core::ElapsedTimer::start_new();

            // Warm up
            out("P\033[1D");
            fflush(stdout);
            run_once(0);

            constexpr char spinner[] {'-', '\\', '|', '/'};
            for (int i = 0; i < num_runs; i++) {
                out("{}\033[1D", spinner[i%4]);
                fflush(stdout);
                run_once(i);
            }
            auto milliseconds = timer.elapsed();

            StringBuilder permutation_string_builder;
            for (auto& parameter: m_parameters) {
                permutation_string_builder.appendff("{} = {}, ", parameter->name(), parameter->value_string());
            }

            auto permutation_string = permutation_string_builder.to_string();

            if (milliseconds > 0)
                outln("{}: {} runs in {}ms. {}ms per run. {} per second", permutation_string, num_runs, milliseconds, static_cast<float>(milliseconds) / num_runs, num_runs * 1000 / milliseconds);
            else
                outln("{}: {} runs in {}ms. {}ms per run.", permutation_string, num_runs, milliseconds, static_cast<float>(milliseconds) / num_runs);


            if (milliseconds > slowest_run) {
                slowest_run = milliseconds;
                slowest_permutation = permutation_string;
            }
            if (milliseconds < fastest_run) {
                fastest_run = milliseconds;
                fastest_permutation = permutation_string;
            }

            shutdown_run();
        } while (advance_parameters());

        outln("---");
        outln("Fastest: {}: {}ms per run", fastest_permutation, static_cast<float>(fastest_run) / num_runs);
        outln("Slowest: {}: {}ms per run", slowest_permutation, static_cast<float>(slowest_run) / num_runs);
        outln("");

        shutdown();
    }

    bool advance_parameters()
    {
        for (auto& parameter: m_parameters) {
            if (parameter->advance())
                return true;
            parameter->reset();
        }
        return false;
    }

    virtual void initialize() {};
    virtual void shutdown() {};
    virtual void initialize_run() {};
    virtual void shutdown_run() {};
    virtual void run_once(int frame_number) = 0;

    StringView name() const { return m_name; }
    StringView description() const { return m_description; }

private:
    String m_name;
    String m_description;
    Vector<Parameter*> m_parameters;
};
