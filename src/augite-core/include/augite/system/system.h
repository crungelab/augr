#pragma once

#include <vector>


class System
{
public:
    virtual ~System() = default;
    virtual void Create() = 0;
    virtual void Destroy() = 0;
};

class SystemContainer
{
public:
    void Add(System *system) { contents_.push_back(system); }
    void Create() const
    {
        for (auto c : contents_)
        {
            c->Create();
        }
    }
    void Destroy() const
    {
        for (auto c : contents_)
        {
            c->Destroy();
        }
    }
    // Data members
    std::vector<System *> contents_;
};