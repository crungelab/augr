#include <augr/archiver_factory.h>
#include <augr/archiver_manufacturer.h>

#include <augite/viewer/rack_viewer.h>
#include <augite/archiver/subrack_viewer_archiver.h>

namespace augr {

class RackViewerArchiver : public SubrackViewerArchiver {};
DEFINE_ARCHIVER_FACTORY(RackViewerArchiver, RackViewer, "RackViewer")

} // namespace augr