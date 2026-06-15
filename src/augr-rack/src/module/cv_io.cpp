#include <augr/core/archiver_factory.h>
#include <augr/core/model_factory.h>

#include <augr/rack/archiver/module_archiver.h>
#include <augr/rack/graph.h>

#include <augr/rack/module/cv_io.h>


namespace augr {

void CvInputModule::OnCreate() {
    Io::OnCreate();
    label_ = "CV Input Module";
}

void CvInputModule::CreatePins() {
    cv_out_ = new VoltageOutput(*this, "cv_out");
    AddOutput(*cv_out_);

    cv_in_ = new VoltageInput(*this, "cv_in");
    graph().AddInput(*cv_in_);
}

void CvInputModule::Process() {
    Voltage cv = cv_in_->Read();
    cv_out_->Write(cv);
}

void CvOutputModule::OnCreate() {
    Io::OnCreate();
    label_ = "CV Output Module";
}

void CvOutputModule::CreatePins() {
    cv_in_ = new VoltageInput(*this, "cv_in");
    AddInput(*cv_in_);

    cv_out_ = new VoltageOutput(graph(), "cv_out");
    graph().AddOutput(*cv_out_);
}

void CvOutputModule::Process() {
    Voltage cv = cv_in_->Read();
    cv_out_->Write(cv);
}

} // namespace augr

using namespace augr;

DEFINE_MODULE(CvInputModule, "CvInputModule", "Io")
DEFINE_MODULE(CvOutputModule, "CvOutputModule", "Io")
