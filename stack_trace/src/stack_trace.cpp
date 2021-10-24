#define BACKWARD_HAS_DW 1

#include "stack_trace/stack_trace.hpp"
#include "stack_trace/backward.hpp"

std::string stacktrace_str(std::size_t stackSize, std::size_t skip)
{
    backward::StackTrace st;
    st.load_here(stackSize);
    st.skip_n_firsts(skip);
    backward::TraceResolver tr;
    tr.load_stacktrace(st);
    std::stringstream ss;
    for (size_t i = 0; i < st.size(); ++i)
    {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        ss << trace.source.filename << ":" << trace.source.line << "\t";
    }
    std::string res = ss.str();
    res.pop_back();
    return res;
}

std::vector<std::string> stacktrace_vec(std::size_t stackSize, std::size_t skip)
{
    backward::StackTrace st;
    st.load_here(stackSize);
    st.skip_n_firsts(skip);
    backward::TraceResolver tr;
    tr.load_stacktrace(st);
    std::vector<std::string> vec;
    for (size_t i = 0; i < st.size(); ++i)
    {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        std::string str(trace.source.filename + ":" + std::to_string(trace.source.line));
        vec.emplace_back(str);
    }
    return vec;
}