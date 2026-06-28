#pragma once

#include <augite/viewer/subrack_viewer.h>

#include "viewer_archiver.h"

namespace augr {

class SubrackViewerArchiver : public ArchiverT<SubrackViewer, ViewerArchiver> {
};

} // namespace augr