#pragma once

#include <AK/NonnullOwnPtr.h>
#include <AK/Vector.h>

#include "Scenario.h"

class Benchmark {
public:
    virtual ~Benchmark() {}

    void add(NonnullOwnPtr<Scenario>&& scenario) { m_scenarios.append(move(scenario)); }

    void run(StringView selected_scenario)
    {
        initialize();
        
        for (auto& scenario : m_scenarios)
            if (selected_scenario.is_empty() || scenario->name() == selected_scenario)
                scenario->run();

        shutdown();
    }

    virtual void initialize() {};
    virtual void shutdown() {};

    Vector<NonnullOwnPtr<Scenario>> const& scenarios() const { return m_scenarios; }

private:
    Vector<NonnullOwnPtr<Scenario>> m_scenarios;
};
