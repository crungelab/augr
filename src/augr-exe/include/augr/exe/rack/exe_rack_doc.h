#pragma once

#include <augr/rack/rack_doc.h>

#include "exe_rack.h"

namespace augr {

class ExeRackDoc : public RackDoc {
public:
    ExeRackDoc() = default;
    ~ExeRackDoc() override = default;
    void NewDocument() override {
        RackDoc::NewDocument();
        rack_ = new ExeRack();
    }
};

} // namespace augr