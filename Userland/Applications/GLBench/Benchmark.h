#pragma once

#include <AK/NonnullOwnPtr.h>
#include <AK/Vector.h>

#include "Scenario.h"

class Benchmark {
public:
    virtual ~Benchmark() {}

    void add(NonnullOwnPtr<Scenario>&& scenario) { m_scenarios.append(move(scenario)); }

    void run()
    {
        initialize();
        for (auto& scenario : m_scenarios)
            scenario->run();
        shutdown();
    }

    virtual void initialize() = 0;
    virtual void shutdown() = 0;

private:
    Vector<NonnullOwnPtr<Scenario>> m_scenarios;
};
