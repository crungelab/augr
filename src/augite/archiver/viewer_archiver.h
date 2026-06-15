#pragma once

#include <augr/core/archiver.h>
#include <augite/viewer/viewer.h>

namespace augr {

class ViewerArchiver : public ArchiverT<Viewer> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;

protected:
    void SaveView(Archive &archive) const;
    void LoadView(Archive &archive);

    void SaveSubviewers(Archive &archive) const;
    void LoadSubviewers(Archive &archive);
};

} // namespace augr