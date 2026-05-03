#include <augite/app/app.h>

class MyApp : public augr::App {
    void Draw();
    // Data members
    bool show_demo_window = true;
    bool show_another_window = false;
};
