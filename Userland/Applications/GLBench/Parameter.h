#pragma once

#include <AK/String.h>
#include <AK/StringView.h>
#include <initializer_list>

class Parameter {
public:
    Parameter(StringView name)
        : m_name{name}
    {
    }

    virtual ~Parameter() {}

    virtual bool advance() = 0;
    virtual void reset() = 0;

    virtual size_t count() const = 0;
    String const& name() const { return m_name; }
    virtual String const& value_string() const = 0;

private:
    String m_name;
};

template<typename T>
class GenericParameter : public Parameter {
public:
    struct ValueNamePair {
        T value;
        String name;
    };

    GenericParameter(StringView name, std::initializer_list<T> initializer)
        : Parameter { name }
    {
        for (auto const& element : initializer) {
            m_options.append({element, String::formatted("{}", element)});
        }
    }

    GenericParameter(StringView name, std::initializer_list<ValueNamePair> initializer)
        : Parameter { name }
    {
        for (auto const& element : initializer) {
            m_options.append({element.value, element.name});
        }
    }

    virtual bool advance() override
    {
        if (m_current_option >= m_options.size() - 1)
            return false;

        m_current_option++;
        return true;
    }

    virtual void reset() override { m_current_option = 0; }

    virtual size_t count() const override { return m_options.size(); }
    T const& value() const { return m_options[m_current_option].value; }
    virtual String const& value_string() const override { return m_options[m_current_option].name; }

private:
    Vector<ValueNamePair> m_options;
    size_t m_current_option { 0 };
};
