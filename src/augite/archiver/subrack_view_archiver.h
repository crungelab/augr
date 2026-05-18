// augite/archiver/subrack_view_archiver.h
#pragma once

#include <augr/core/archiver.h>
#include <augite/view/subrack_view.h>

namespace augr {

class SubrackViewArchiver : public ArchiverT<SubrackView> {
public:
    void Save(Archive& archive) const override;
    void Load(Archive& archive) override;

//private:
    //void SaveWidgets(Archive& archive) const;
    void SaveWidgets(Archive& archive, const std::vector<Widget *> &widgets) const;
    void LoadWidgets(Archive& archive);
};

} // namespace augr