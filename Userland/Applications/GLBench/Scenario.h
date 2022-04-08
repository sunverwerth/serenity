#pragma once

#include <AK/String.h>
#include <LibCore/ElapsedTimer.h>

#include "Parameter.h"

class Scenario {
public:
    Scenario(StringView name)
        : m_name{name}
    {
    }

    virtual ~Scenario() {}

    void add(Parameter& parameter) { m_parameters.append(&parameter); }
    
    void run()
    {
        dbgln("=== Running scenario '{}'", m_name);
        initialize();
        do {
            initialize_run();
            auto timer = Core::ElapsedTimer::start_new();
            constexpr int num_runs = 100;
            for (int i = 0; i < num_runs; i++)
                run_once();
            auto milliseconds = timer.elapsed();

            StringBuilder line;
            for (auto& parameter: m_parameters) {
                line.appendff("{} = {}, ", parameter->name(), parameter->value_string());
            }
            line.appendff(": {} runs in {}ms. {}ms per run", num_runs, milliseconds, static_cast<float>(milliseconds) / num_runs);
            dbgln(line.to_string());

            shutdown_run();
        } while (advance_parameters());
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
    virtual void run_once() {};

private:
    String m_name;
    Vector<Parameter*> m_parameters;
};
