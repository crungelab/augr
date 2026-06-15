#pragma once

#include <augite/viewer/subrack_viewer.h>

#include "viewer_archiver.h"

namespace augr {

class SubrackViewerArchiver : public ArchiverT<SubrackViewer, ViewerArchiver> {
/*
public:
    void Save(Archive &archive) const override;
    void Load(Archive &archive) override;

private:
    void SaveSubviewers(Archive &archive) const;
    void LoadSubviewers(Archive &archive);
*/

};
} // namespace augr