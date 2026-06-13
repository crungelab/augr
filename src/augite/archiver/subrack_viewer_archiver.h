#pragma once

#include <augr/core/archiver.h>
#include <augite/viewer/subrack_viewer.h>

namespace augr {

class SubrackViewerArchiver : public ArchiverT<SubrackViewer> {
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;

private:
    void SaveView(Archive &archive) const;
    void LoadView(Archive &archive);
};

} // namespace augr