#pragma once

#include <augr/core/archiver.h>
#include <augite/viewer/viewer.h>

namespace augr {

class ViewerArchiver : public ArchiverT<Viewer> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;

private:
    void SaveView(Archive &archive) const;
    void LoadView(Archive &archive);
};

} // namespace augr