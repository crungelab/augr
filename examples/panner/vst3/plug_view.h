#include <augr/vst3/plug_view.h>

namespace Steinberg {
namespace Panner {

class PannerEditor;

class PannerView : public PlugView {
public:
	PannerView(PannerEditor* editor, ViewRect* size);
	virtual void attachedToParent() override;
};

}
}