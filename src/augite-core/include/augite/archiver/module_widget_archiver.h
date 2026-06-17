// augite/archiver/module_widget_archiver.h
#pragma once

#include <augr/archiver.h>
#include <augr/archiver_factory.h>

#include <augite/widget/module_widget.h>

namespace augr {

class ModuleWidgetArchiver : public ArchiverT<ModuleWidget> {
public:
    void Save(Archive& archive) const override;
    void Load(Archive& archive) override;
};

} // namespace augr