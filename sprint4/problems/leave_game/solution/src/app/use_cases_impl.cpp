#include "use_cases_impl.h"

namespace app {

void UseCasesImpl::SaveRetiredPlayers(const data_base::domain::RetiredPlayers& records) {
    auto work = unit_factory_.CreateUnitOfWork();
    work->RetiredPlayers().SaveAll(records);
    work->Commit();
}

data_base::domain::RetiredPlayers UseCasesImpl::GetRetiredPlayers(size_t offset, size_t limit) {
    auto work = unit_factory_.CreateUnitOfWork();
    return work->RetiredPlayers().Get(offset, limit);
}

}