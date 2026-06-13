#include <augr/core/archiver_factory.h>
#include <augr/core/archiver_manufacturer.h>

#include "../viewer/rack_viewer.h"
#include "subrack_viewer_archiver.h"

namespace augr {

class RackViewerArchiver : public SubrackViewerArchiver {};
DEFINE_ARCHIVER_FACTORY(RackViewerArchiver, RackViewer, "RackViewer")

} // namespace augr