#pragma once

#include "app.h"

#include <augr/rack/rack.h>
#include <augr/rack/rack_doc.h>

namespace augr {

class RackView;

class RackApp : public App {
public:
    RackApp();

    // Accessors
    Rack &rack() { return doc_.rack(); }

    // Data members
    static RackApp *singleton_;
    RackDoc doc_;
};

} // namespace augr